#pragma once

#include <cstdint>

// Fase 1: pines de la interfaz RFID (RDM6300 por UART, sólo lectura).
namespace pins {
constexpr int RFID_RX = 16;           // GPIO16 (RX2) <- TX del RDM6300, conexión directa (sin divisor), como el diagrama oficial de la librería rdm6300
constexpr uint8_t RFID_UART_NUM = 2;  // controlador UART hardware nº2 del ESP32

// Fase 4: pines de la interfaz QR (GROW GM861S por UART, 3.3V nativo, sin divisor).
constexpr int QR_RX = 4;              // GPIO4 <- TXD del lector QR (cable negro en el módulo de repuesto; el azul es D- de USB, sin usar)
constexpr uint8_t QR_UART_NUM = 1;    // controlador UART hardware nº1 del ESP32
constexpr uint32_t QR_BAUD_RATE = 9600;  // valor de fábrica del GM861S (Form 2-1 del manual); el módulo de repuesto no se ha reconfigurado a otro baudrate
}  // namespace pins
