#pragma once

#include <cstdint>

// Fase 1: pines de la interfaz RFID (RDM6300 por UART, sólo lectura).
namespace pins {
constexpr int RFID_RX = 16;           // GPIO16 (RX2) <- TX del RDM6300, vía divisor de tensión 5V->3.3V
constexpr uint8_t RFID_UART_NUM = 2;  // controlador UART hardware nº2 del ESP32

// Fase 4: pines de la interfaz QR (GROW GM861S por UART, 3.3V nativo, sin divisor).
constexpr int QR_RX = 4;              // GPIO4 <- TXD del lector QR (cable azul)
constexpr uint8_t QR_UART_NUM = 1;    // controlador UART hardware nº1 del ESP32
constexpr uint32_t QR_BAUD_RATE = 57600;
}  // namespace pins
