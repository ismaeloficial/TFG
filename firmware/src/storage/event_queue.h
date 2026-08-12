#pragma once

#include <Arduino.h>

// Cola FIFO persistente (en la flash del ESP32, vía LittleFS) de payloads
// pendientes de publicar por MQTT. Cada entrada es una línea de texto (el
// JSON ya construido). Sobrevive a cortes de red y a reinicios del propio
// ESP32 — el objetivo de la Fase 3 es no perder eventos aunque falle la red.
class EventQueue {
 public:
  // Monta LittleFS. Llamar una vez en setup(). Si el montaje falla, la cola
  // queda desactivada de forma silenciosa (push()/peekFront() no hacen nada)
  // en vez de bloquear el resto del firmware.
  bool begin();

  // Añade un payload al final de la cola.
  void push(const String &payload);

  bool hasPending();

  // Devuelve en outPayload el evento más antiguo sin quitarlo de la cola
  // todavía. false si no hay nada pendiente.
  bool peekFront(String &outPayload);

  // Quita de la cola el evento más antiguo. Llamar solo tras confirmar que
  // se ha reenviado correctamente.
  void popFront();

 private:
  bool mounted_ = false;
};
