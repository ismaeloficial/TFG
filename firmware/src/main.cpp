#include <Arduino.h>

#include "config/pins.h"
#include "events/event.h"
#include "network/network_manager.h"
#include "qr/qr_reader.h"
#include "rfid/rfid_reader.h"

RfidReader rfidReader;
QrReader qrReader;
NetworkManager networkManager;

void setup() {
  Serial.begin(115200);
  delay(200);
  rfidReader.begin(pins::RFID_RX, pins::RFID_UART_NUM);
  qrReader.begin(pins::QR_RX, pins::QR_UART_NUM, pins::QR_BAUD_RATE);

  if (!networkManager.begin()) {
    Serial.println("Aviso: red no disponible, los eventos solo saldrán por Serial.");
  }

  Serial.println("Acerca una etiqueta RFID o escanea un QR...");
}

void loop() {
  networkManager.loop();

  Event event;
  if (rfidReader.poll(event)) {
    printEvent(event);
    networkManager.publishEvent(event);
  }
  if (qrReader.poll(event)) {
    printEvent(event);
    networkManager.publishEvent(event);
  }
  delay(10);
}
