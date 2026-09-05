#pragma once

#include <HardwareSerial.h>

#include "../events/event.h"

// Lee líneas de texto ASCII que el módulo GM861S envía por UART al decodificar un código.
class QrReader {
 public:
  void begin(int rx_pin, uint8_t uart_num, uint32_t baud_rate);

  // Si hay una línea completa decodificada, rellena `event` y devuelve true.
  bool poll(Event &event);

 private:
  HardwareSerial *serial_ = nullptr;
  char buffer_[sizeof(Event::qr_data)];
  uint8_t buffer_len_ = 0;

  // Anti-rebote: en Continuous Mode el módulo vuelve a leer el mismo código
  // varias veces por segundo mientras siga delante del lector. Se ignoran
  // relecturas del mismo código dentro de kDuplicateCooldownMs (ver .cpp).
  char lastCode_[sizeof(Event::qr_data)] = {0};
  uint32_t lastReadMs_ = 0;
};
