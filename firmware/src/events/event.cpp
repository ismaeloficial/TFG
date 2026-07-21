#include "event.h"

namespace {

const char *eventTypeName(EventType type) {
  switch (type) {
    case EventType::RFID:
      return "RFID";
    case EventType::QR:
      return "QR";
  }
  return "UNKNOWN";
}

}  // namespace

void printEvent(const Event &event) {
  Serial.print("{\"tag_id\":\"");
  Serial.print(event.tag_id, HEX);
  Serial.print("\",\"type\":\"");
  Serial.print(eventTypeName(event.type));
  Serial.print("\",\"timestamp_ms\":");
  Serial.print(event.timestamp_ms);
  Serial.println("}");
}
