#include <Arduino.h>

#include "config/pins.h"
#include "events/event.h"
#include "led/confirmation_led.h"
#include "network/network_manager.h"
#include "qr/qr_reader.h"
#include "rfid/rfid_reader.h"

RfidReader rfidReader;
QrReader qrReader;
NetworkManager networkManager;
ConfirmationLed confirmationLed;

// Publica el evento y señaliza el LED según el resultado: 3 parpadeos cortos
// si se ha subido al momento, 1 parpadeo largo si se ha quedado en la cola
// local a la espera de red (el reenvío posterior de la cola se señaliza
// aparte, desde el propio NetworkManager — ver setConfirmationLed()).
void publishAndSignal(const Event &event) {
  if (networkManager.publishEvent(event)) {
    confirmationLed.showSuccess();
  } else {
    confirmationLed.showQueued();
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  rfidReader.begin(pins::RFID_RX, pins::RFID_UART_NUM);
  qrReader.begin(pins::QR_RX, pins::QR_UART_NUM, pins::QR_BAUD_RATE);
  confirmationLed.begin(pins::LED_PIN);
  networkManager.setConfirmationLed(&confirmationLed);

  if (!networkManager.begin()) {
    Serial.println("Aviso: red no disponible, los eventos solo saldrán por Serial.");
  }

  Serial.println("Acerca una etiqueta RFID o escanea un QR...");
}

void loop() {
  networkManager.loop();
  confirmationLed.loop();

  Event event;
  if (rfidReader.poll(event)) {
    printEvent(event);
    publishAndSignal(event);
  }
  if (qrReader.poll(event)) {
    printEvent(event);
    publishAndSignal(event);
  }
  delay(10);
}
