#include <Arduino.h>

#include "config/pins.h"
#include "events/event.h"
#include "rfid/rfid_reader.h"

RfidReader rfidReader;

void setup() {
  Serial.begin(115200);
  rfidReader.begin(pins::RFID_RX, pins::RFID_UART_NUM);
  Serial.println("Acerca una etiqueta RFID al lector...");
}

void loop() {
  Event event;
  if (rfidReader.poll(event)) {
    printEvent(event);
  }
  delay(10);
}
