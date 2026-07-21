#pragma once

#include <cstdint>

// Fase 1: pines de la interfaz RFID (RDM6300 por UART, sólo lectura).
namespace pins {
constexpr int RFID_RX = 16;           // GPIO16 (RX2) <- TX del RDM6300, vía divisor de tensión 5V->3.3V
constexpr uint8_t RFID_UART_NUM = 2;  // controlador UART hardware nº2 del ESP32
}  // namespace pins
