#include "qr_reader.h"

#include <string.h>

void QrReader::begin(int rx_pin, uint8_t uart_num, uint32_t baud_rate) {
  serial_ = new HardwareSerial(uart_num);
  serial_->begin(baud_rate, SERIAL_8N1, rx_pin, -1);
}

bool QrReader::poll(Event &event) {
  while (serial_->available()) {
    char c = serial_->read();
    if (c != '\r' && c != '\n') {
      if (buffer_len_ < sizeof(buffer_) - 1) {
        buffer_[buffer_len_++] = c;
      }
      continue;
    }

    if (buffer_len_ == 0) {
      continue;  // salto de línea suelto, sin datos acumulados
    }

    buffer_[buffer_len_] = '\0';
    strncpy(event.qr_data, buffer_, sizeof(event.qr_data) - 1);
    event.qr_data[sizeof(event.qr_data) - 1] = '\0';
    event.tag_id = 0;
    event.type = EventType::QR;
    event.timestamp_ms = millis();
    buffer_len_ = 0;
    return true;
  }
  return false;
}
