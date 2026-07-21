#pragma once

#include <Arduino.h>

enum class EventType : uint8_t {
  RFID = 0,
  QR = 1,
};

struct Event {
  uint32_t tag_id;
  EventType type;
  uint32_t timestamp_ms;  // millis() desde el arranque; se reconciliará con hora real (NTP) en fases posteriores.
};

// Saca el evento por Serial en un formato de línea única (provisional para Fase 1).
// Fase 2 lo sustituye por un payload JSON publicado por MQTT.
void printEvent(const Event &event);
