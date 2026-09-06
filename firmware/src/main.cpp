#include <Arduino.h>
#include <esp_system.h>

#include "config/pins.h"
#include "events/event.h"
#include "network/network_manager.h"
#include "qr/qr_reader.h"
#include "rfid/rfid_reader.h"

RfidReader rfidReader;
QrReader qrReader;
NetworkManager networkManager;

namespace {
// Diagnóstico temporal: imprime por qué se reinició la placa la última vez.
// Si aparece "BROWNOUT" de forma repetida justo antes de fallar el WiFi, el
// problema es de alimentación (el pico de corriente de la radio al conectar),
// no de configuración de red. Quitar una vez localizada la causa real.
void printResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Motivo del último reinicio: ");
  switch (reason) {
    case ESP_RST_POWERON: Serial.println("POWERON (encendido normal)"); break;
    case ESP_RST_BROWNOUT: Serial.println("BROWNOUT (caída de tensión — problema de alimentación)"); break;
    case ESP_RST_PANIC: Serial.println("PANIC (excepción de software / crash)"); break;
    case ESP_RST_INT_WDT: Serial.println("INT_WDT (watchdog de interrupción)"); break;
    case ESP_RST_TASK_WDT: Serial.println("TASK_WDT (watchdog de tarea)"); break;
    case ESP_RST_WDT: Serial.println("WDT (otro watchdog)"); break;
    case ESP_RST_SW: Serial.println("SW (reinicio por software)"); break;
    case ESP_RST_DEEPSLEEP: Serial.println("DEEPSLEEP"); break;
    default: Serial.printf("otro (código %d)\n", (int)reason); break;
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  printResetReason();
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
