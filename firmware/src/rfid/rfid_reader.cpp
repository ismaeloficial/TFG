#include "rfid_reader.h"

void RfidReader::begin(int rx_pin, uint8_t uart_num) {
  rdm6300_.begin(rx_pin, uart_num);
}

bool RfidReader::poll(Event &event) {
  uint32_t tag_id = rdm6300_.get_new_tag_id();
  if (!tag_id) {
    return false;
  }
  event.tag_id = tag_id;
  event.type = EventType::RFID;
  event.timestamp_ms = millis();
  return true;
}
