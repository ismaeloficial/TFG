#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

#include "../events/event.h"

// Gestiona WiFi, sincronización horaria (NTP) y publicación de eventos por MQTT.
class NetworkManager {
 public:
  // Conecta WiFi, sincroniza la hora y conecta al broker MQTT. Bloqueante, con
  // timeout en cada paso; devuelve false si alguno falla (detalle por Serial).
  bool begin();

  // Llamar en cada vuelta de loop() — mantiene viva la conexión MQTT y
  // reconecta sola si se cae.
  void loop();

  // Publica un evento en el topic configurado. Devuelve false si no hay
  // conexión MQTT activa (el evento no se pierde del todo: sigue saliendo
  // por Serial vía printEvent, solo no llega al backend).
  bool publishEvent(const Event &event);

 private:
  bool connectWifi();
  bool syncTime();
  bool connectMqtt();

  WiFiClient wifiClient_;
  PubSubClient mqttClient_{wifiClient_};
};
