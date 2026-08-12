#include "network_manager.h"

#include <time.h>

#include "../config/network_config.h"

namespace {
constexpr uint32_t kWifiTimeoutMs = 20000;
constexpr uint32_t kTimeSyncTimeoutMs = 15000;
// ~noviembre 2023 en epoch; sirve para distinguir "hora ya sincronizada" de
// "hora todavía en 1970" mientras el NTP no ha respondido.
constexpr time_t kMinValidEpoch = 1700000000;
// Cuánto esperar entre reintentos de WiFi/hora/MQTT dentro de loop(). Sin
// este límite, un intento de conexión bloqueante en cada vuelta del bucle
// puede dejar sin CPU al sondeo de RFID/QR mientras no haya red disponible.
constexpr uint32_t kReconnectIntervalMs = 5000;
// Cada cuánto se intenta reenviar (como mucho un evento) de la cola local.
constexpr uint32_t kQueueFlushIntervalMs = 2000;
// Margen de gracia tras reconectar MQTT antes de empezar a vaciar la cola.
// La ESP32 y el backend reconectan cada uno por su cuenta, sin coordinarse;
// si el primer evento se reenvía antes de que el backend se haya vuelto a
// suscribir, se pierde sin más (QoS 0 no avisa ni reintenta). Esta espera
// le da tiempo al backend a ponerse al día.
constexpr uint32_t kQueueFlushGraceMs = 4000;
}  // namespace

bool NetworkManager::begin() {
  eventQueue_.begin();  // si falla, sigue funcionando sin cola persistente (aviso ya impreso dentro)

  if (!connectWifi()) {
    Serial.println("NetworkManager: fallo al conectar WiFi (se seguirá reintentando en segundo plano)");
    return false;
  }
  timeSynced_ = syncTime();
  if (!timeSynced_) {
    Serial.println("NetworkManager: fallo al sincronizar hora (se reintentará más adelante)");
  }
  if (!connectMqtt()) {
    Serial.println("NetworkManager: fallo al conectar MQTT");
    return false;
  }
  return true;
}

bool NetworkManager::connectWifi() {
  Serial.print("Conectando a WiFi '");
  Serial.print(network_config::WIFI_SSID);
  Serial.println("'...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(network_config::WIFI_SSID, network_config::WIFI_PASSWORD);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > kWifiTimeoutMs) {
      return false;
    }
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi conectado, IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool NetworkManager::syncTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  uint32_t start = millis();
  while (time(nullptr) < kMinValidEpoch) {
    if (millis() - start > kTimeSyncTimeoutMs) {
      return false;
    }
    delay(250);
  }
  Serial.println("Hora sincronizada por NTP");
  return true;
}

bool NetworkManager::connectMqtt() {
  mqttClient_.setServer(network_config::MQTT_BROKER_HOST, network_config::MQTT_BROKER_PORT);

  String clientId = "esp32-trazabilidad-" + WiFi.macAddress();
  if (!mqttClient_.connect(clientId.c_str())) {
    return false;
  }
  Serial.println("MQTT conectado");
  mqttConnectedSinceMs_ = millis();
  return true;
}

bool NetworkManager::publishRaw(const String &payload) {
  if (!mqttClient_.connected()) return false;
  return mqttClient_.publish(network_config::MQTT_TOPIC_EVENTS, payload.c_str());
}

void NetworkManager::flushQueueIfAny() {
  if (!mqttClient_.connected() || !eventQueue_.hasPending()) return;

  if (millis() - mqttConnectedSinceMs_ < kQueueFlushGraceMs) return;

  String payload;
  if (!eventQueue_.peekFront(payload)) return;

  if (publishRaw(payload)) {
    eventQueue_.popFront();
    Serial.println("Evento pendiente de la cola local reenviado correctamente");
  }
  // Si falla, se deja en la cola tal cual — se reintenta en la siguiente vuelta.
}

void NetworkManager::loop() {
  uint32_t now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    // Sin WiFi no tiene sentido ni intentar MQTT. Reintenta como mucho cada
    // kReconnectIntervalMs y vuelve enseguida — el resto de loop() (RFID/QR)
    // sigue funcionando aunque la red esté caída; lo no publicado se queda
    // en la cola local hasta que vuelva la conexión.
    if (now - lastWifiRetryMs_ > kReconnectIntervalMs) {
      lastWifiRetryMs_ = now;
      Serial.println("WiFi desconectado, reintentando...");
      WiFi.reconnect();
    }
    return;
  }

  // Si el arranque ocurrió sin red disponible, la hora nunca llegó a
  // sincronizarse en begin() — se reintenta aquí en cuanto hay WiFi.
  if (!timeSynced_ && now - lastTimeSyncRetryMs_ > kReconnectIntervalMs) {
    lastTimeSyncRetryMs_ = now;
    timeSynced_ = syncTime();
  }

  if (!mqttClient_.connected()) {
    if (now - lastMqttRetryMs_ > kReconnectIntervalMs) {
      lastMqttRetryMs_ = now;
      connectMqtt();
    }
    return;
  }

  mqttClient_.loop();

  if (now - lastQueueFlushMs_ > kQueueFlushIntervalMs) {
    lastQueueFlushMs_ = now;
    flushQueueIfAny();
  }
}

bool NetworkManager::publishEvent(const Event &event) {
  char rawValue[sizeof(Event::qr_data)];
  if (event.type == EventType::RFID) {
    snprintf(rawValue, sizeof(rawValue), "%X", event.tag_id);
  } else {
    strncpy(rawValue, event.qr_data, sizeof(rawValue) - 1);
    rawValue[sizeof(rawValue) - 1] = '\0';
  }

  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  char readAt[25];
  strftime(readAt, sizeof(readAt), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);

  char payloadBuf[192];
  snprintf(payloadBuf, sizeof(payloadBuf),
           "{\"raw_value\":\"%s\",\"read_type\":\"%s\",\"read_at\":\"%s\",\"read_point\":\"%s\"}",
           rawValue, eventTypeName(event.type), readAt, network_config::READ_POINT);
  String payload(payloadBuf);

  if (publishRaw(payload)) {
    return true;
  }

  Serial.println("Sin conexión MQTT: evento guardado en la cola local");
  eventQueue_.push(payload);
  return false;
}
