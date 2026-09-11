#pragma once

#include <Arduino.h>

// LED de confirmación visual para la presentación: un patrón cuando un
// evento se guarda en la cola local (sin conexión inmediata), y otro cuando
// un evento llega con éxito a la base de datos, ya sea al momento o al
// reenviarse más tarde desde la cola.
//
// No bloqueante: begin()/loop() siguen el mismo patrón que el resto del
// firmware (ver NetworkManager). Usar delay() aquí congelaría el sondeo de
// RFID/QR mientras el LED parpadea, que es justo lo que no queremos.
class ConfirmationLed {
 public:
  void begin(int pin);

  // Llamar en cada vuelta de loop() para avanzar el patrón en curso.
  void loop();

  // Arranca el patrón de "evento guardado en la cola local" (1 parpadeo de 2s).
  void showQueued();

  // Arranca el patrón de "evento subido con éxito" (3 parpadeos de 0.5s).
  void showSuccess();

 private:
  enum class Pattern { kNone, kQueued, kSuccess };

  int pin_ = -1;
  Pattern pattern_ = Pattern::kNone;
  uint8_t blinksDone_ = 0;    // en qué parpadeo del patrón actual vamos (solo kSuccess)
  uint32_t stepStartMs_ = 0;  // cuándo empezó el paso actual (encendido u apagado)
  bool ledOn_ = false;

  void startPattern(Pattern pattern);
  void setLed(bool on);
};
