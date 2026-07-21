#pragma once

#include <rdm6300.h>

#include "../events/event.h"

// Envoltorio fino sobre Rdm6300 para aislar el resto del firmware de la librería concreta.
class RfidReader {
 public:
  void begin(int rx_pin, uint8_t uart_num);

  // Si hay una etiqueta nueva junto al lector, rellena `event` y devuelve true.
  bool poll(Event &event);

 private:
  Rdm6300 rdm6300_;
};
