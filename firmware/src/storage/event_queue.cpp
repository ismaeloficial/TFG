#include "event_queue.h"

#include <LittleFS.h>

namespace {
constexpr const char *kQueuePath = "/queue.jsonl";
constexpr const char *kTempPath = "/queue.tmp";
}  // namespace

bool EventQueue::begin() {
  // El "true" formatea la partición si el montaje falla (p.ej. primer uso).
  mounted_ = LittleFS.begin(true);
  if (!mounted_) {
    Serial.println("EventQueue: fallo al montar LittleFS, la cola local queda desactivada");
  }
  return mounted_;
}

void EventQueue::push(const String &payload) {
  if (!mounted_) return;

  // FILE_APPEND en LittleFS/ESP32 no crea el archivo si todavía no existe
  // (a pesar de lo que sugiere el nombre) — hay que crearlo explícitamente
  // la primera vez antes de poder abrirlo en modo "append".
  if (!LittleFS.exists(kQueuePath)) {
    File created = LittleFS.open(kQueuePath, FILE_WRITE);
    if (!created) {
      Serial.println("EventQueue: no se pudo crear el archivo de la cola");
      return;
    }
    created.close();
  }

  File f = LittleFS.open(kQueuePath, FILE_APPEND);
  if (!f) {
    Serial.println("EventQueue: no se pudo abrir la cola para escribir");
    return;
  }
  f.println(payload);
  f.close();
}

bool EventQueue::hasPending() {
  if (!mounted_) return false;

  // Se intenta abrir directamente en vez de comprobar antes con exists():
  // si el archivo no existe todavía (cola vacía, caso normal), open()
  // simplemente devuelve un File inválido — no hace falta tratarlo como error.
  File f = LittleFS.open(kQueuePath, FILE_READ);
  if (!f) return false;
  bool pending = f.available();
  f.close();
  return pending;
}

bool EventQueue::peekFront(String &outPayload) {
  if (!mounted_) return false;

  File f = LittleFS.open(kQueuePath, FILE_READ);
  if (!f) return false;

  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      outPayload = line;
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}

void EventQueue::popFront() {
  if (!mounted_) return;

  File in = LittleFS.open(kQueuePath, FILE_READ);
  if (!in) return;
  File out = LittleFS.open(kTempPath, FILE_WRITE);
  if (!out) {
    in.close();
    return;
  }

  bool skippedFirst = false;
  while (in.available()) {
    String line = in.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    if (!skippedFirst) {
      skippedFirst = true;  // esta es la línea ya enviada: se descarta
      continue;
    }
    out.println(line);
  }
  in.close();
  out.close();

  LittleFS.remove(kQueuePath);
  LittleFS.rename(kTempPath, kQueuePath);
}
