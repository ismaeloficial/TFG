#pragma once

#include <Arduino.h>

enum class EventType : uint8_t {
  RFID = 0,
  QR = 1,
};

struct Event {
  uint32_t tag_id;        // usado cuando type == RFID
  char qr_data[64];       // usado cuando type == QR (texto decodificado, terminado en '\0')
  EventType type;
  uint32_t timestamp_ms;  // millis() desde el arranque; se reconciliará con hora real (NTP) en fases posteriores.
};

// Saca el evento por Serial en un formato de línea única (útil para depuración).
void printEvent(const Event &event);

// "RFID" | "QR" | "UNKNOWN". Se usa también al construir el payload MQTT.
const char *eventTypeName(EventType type);
