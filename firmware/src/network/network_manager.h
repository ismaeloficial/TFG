#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

#include "../events/event.h"
#include "../led/confirmation_led.h"
#include "../storage/event_queue.h"

// Gestiona WiFi, sincronización horaria (NTP) y publicación de eventos por MQTT.
class NetworkManager {
 public:
  // Conecta WiFi, sincroniza la hora y conecta al broker MQTT. Bloqueante, con
  // timeout en cada paso; devuelve false si alguno falla (detalle por Serial).
  bool begin();

  // Llamar en cada vuelta de loop() — mantiene viva la conexión MQTT y
  // reconecta sola si se cae.
  void loop();

  // Publica un evento en el topic configurado. Si no hay conexión MQTT en
  // ese momento, el evento se guarda en la cola local (LittleFS) en vez de
  // perderse, y se reenvía solo más adelante cuando vuelva la red. Devuelve
  // true solo si se publicó en el momento (false = publicado más tarde, o
  // sin cola disponible).
  bool publishEvent(const Event &event);

  // Asocia el LED de confirmación, para poder señalizarlo cuando un evento
  // pendiente de la cola local se reenvía con éxito más tarde (ver
  // flushQueueIfAny()) — el caso de "publicado al momento" ya lo puede
  // distinguir quien llame a publishEvent() por su valor de retorno, sin
  // necesidad de pasar por aquí.
  void setConfirmationLed(ConfirmationLed *led) { led_ = led; }

 private:
  bool connectWifi();
  bool syncTime();
  bool connectMqtt();
  bool publishRaw(const String &payload);
  void flushQueueIfAny();

  WiFiClient wifiClient_;
  PubSubClient mqttClient_{wifiClient_};
  EventQueue eventQueue_;
  bool timeSynced_ = false;
  uint32_t lastWifiRetryMs_ = 0;
  uint32_t lastMqttRetryMs_ = 0;
  uint32_t lastTimeSyncRetryMs_ = 0;
  uint32_t lastQueueFlushMs_ = 0;
  // Instante (millis()) en el que MQTT se conectó por última vez. Se usa para
  // dar un margen de gracia antes de vaciar la cola local — ver flushQueueIfAny().
  uint32_t mqttConnectedSinceMs_ = 0;
  // true mientras el WiFi está caído; permite imprimir un aviso una sola vez
  // al recuperarse, en vez de no decir nada cuando la reconexión sí funciona.
  bool wifiWasDown_ = false;
  // Opcional: si está asociado (ver setConfirmationLed()), se avisa aquí
  // cuando un evento de la cola local se reenvía con éxito. nullptr por
  // defecto para no obligar a nadie a tener un LED conectado.
  ConfirmationLed *led_ = nullptr;
};
