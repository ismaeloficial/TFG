#include "confirmation_led.h"

namespace {
constexpr uint32_t kQueuedOnMs = 2000;   // 1 parpadeo de 2s: evento a la cola local
constexpr uint32_t kSuccessOnMs = 500;   // 3 parpadeos de 0.5s: evento subido con éxito
constexpr uint32_t kSuccessOffMs = 250;  // hueco entre esos 3 parpadeos
constexpr uint8_t kSuccessBlinks = 3;
}  // namespace

void ConfirmationLed::begin(int pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  setLed(false);
}

void ConfirmationLed::setLed(bool on) {
  ledOn_ = on;
  digitalWrite(pin_, on ? HIGH : LOW);
}

void ConfirmationLed::startPattern(Pattern pattern) {
  // Un patrón nuevo siempre interrumpe al que estuviera en curso: el evento
  // más reciente es el que importa dejar confirmado en el LED.
  pattern_ = pattern;
  blinksDone_ = 0;
  stepStartMs_ = millis();
  setLed(true);
}

void ConfirmationLed::showQueued() {
  startPattern(Pattern::kQueued);
}

void ConfirmationLed::showSuccess() {
  startPattern(Pattern::kSuccess);
}

void ConfirmationLed::loop() {
  if (pattern_ == Pattern::kNone) return;

  uint32_t elapsed = millis() - stepStartMs_;

  if (pattern_ == Pattern::kQueued) {
    if (elapsed >= kQueuedOnMs) {
      setLed(false);
      pattern_ = Pattern::kNone;
    }
    return;
  }

  // Pattern::kSuccess: alterna encendido/apagado kSuccessBlinks veces.
  uint32_t limit = ledOn_ ? kSuccessOnMs : kSuccessOffMs;
  if (elapsed < limit) return;

  stepStartMs_ = millis();
  if (ledOn_) {
    setLed(false);
    blinksDone_++;
    if (blinksDone_ >= kSuccessBlinks) {
      pattern_ = Pattern::kNone;
    }
  } else {
    setLed(true);
  }
}
