#include "network_manager.h"

#include <time.h>

#include "../config/network_config.h"

namespace {
constexpr uint32_t kWifiTimeoutMs = 20000;
constexpr uint32_t kTimeSyncTimeoutMs = 15000;
// ~noviembre 2023 en epoch; sirve para distinguir "hora ya sincronizada" de
// "hora todavía en 1970" mientras el NTP no ha respondido.
constexpr time_t kMinValidEpoch = 1700000000;
}  // namespace

bool NetworkManager::begin() {
  if (!connectWifi()) {
    Serial.println("NetworkManager: fallo al conectar WiFi");
    return false;
  }
  if (!syncTime()) {
    Serial.println("NetworkManager: fallo al sincronizar hora (NTP)");
    return false;
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
  return true;
}

void NetworkManager::loop() {
  if (!mqttClient_.connected()) {
    connectMqtt();
  }
  mqttClient_.loop();
}

bool NetworkManager::publishEvent(const Event &event) {
  if (!mqttClient_.connected()) {
    return false;
  }

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

  char payload[192];
  snprintf(payload, sizeof(payload),
           "{\"raw_value\":\"%s\",\"read_type\":\"%s\",\"read_at\":\"%s\",\"read_point\":\"%s\"}",
           rawValue, eventTypeName(event.type), readAt, network_config::READ_POINT);

  bool ok = mqttClient_.publish(network_config::MQTT_TOPIC_EVENTS, payload);
  if (!ok) {
    Serial.println("Aviso: fallo al publicar evento por MQTT");
  }
  return ok;
}
