# Plan de desarrollo — Sistema embebido de trazabilidad de cajas de fruta

Fecha de referencia: 2026-07-18. Límite normativo: 300 horas de trabajo, entrega máxima convocatoria de septiembre de 2026.

Prioridad general: dejar un MVP end-to-end (RFID → MQTT → backend → Supabase) funcionando lo antes posible, y añadir QR, robustez offline y frontend como capas encima. Si el tiempo aprieta cerca de la entrega, lo primero que se recorta es el frontend (explícitamente fuera del alcance núcleo) y lo último la validación/documentación (son objetivos evaluables del TFG).

## Fase 0 — Entorno y esqueleto (≈15h)
- Proyecto PlatformIO en `firmware/`: placa ESP32 seleccionada, compila y sube un blink de prueba.
- Proyecto Supabase creado: definir esquema inicial (tabla `boxes`, tabla `events` con id, box_id, tipo_lectura [RFID|QR], timestamp, punto_lectura).
- Backend `backend/` arrancando con FastAPI, conexión a Supabase (Postgres) verificada con una query simple.
- Broker MQTT local para desarrollo (Mosquitto en Docker) y prueba de publish/subscribe manual.

**Entregable:** "hola mundo" de cada pieza (firmware sube, backend conecta a Supabase, MQTT publica/recibe).

## Fase 1 — Captura RFID (≈30h)
- Cableado del lector RFID HF (p.ej. MFRC522) al ESP32.
- Lectura de UID por Serial, sin red todavía.
- Modelo de evento en `firmware/src/events/`: id leído, tipo, timestamp (NTP o millis + sync posterior).

**Entregable:** el ESP32 detecta una etiqueta y saca el evento por Serial.

## Fase 2 — Backend y persistencia (≈35h)
- `backend/app/mqtt/`: suscriptor que escucha el topic de eventos, valida el payload (Pydantic) y lo inserta en Supabase.
- `backend/app/api/`: endpoint mínimo de consulta (listar eventos por caja) — sienta base para el futuro frontend.
- Firmware publica el evento RFID por MQTT en vez de solo Serial.

**Entregable:** una lectura RFID real llega de punta a punta hasta una fila en Supabase, consultable por API.

## Fase 3 — Robustez ante desconexión (≈25h)
- Cola local en el ESP32 (`firmware/src/storage/`, LittleFS o NVS) para eventos generados sin WiFi/MQTT disponible.
- Detección de reconexión y vaciado de cola (reenvío en orden, evitando duplicados).
- Prueba: desconectar WiFi a propósito, generar lecturas, reconectar y verificar que todo llega.

**Entregable:** el sistema no pierde eventos aunque falle la red temporalmente (requisito explícito de la memoria).

## Fase 4 — Lectura QR por cámara (≈35h)
- Selección e integración del módulo de cámara (ESP32-CAM u otro).
- Captura de frame + decodificación QR en `firmware/src/qr/`.
- Unificación con el modelo de evento existente (tipo_lectura = QR), mismo camino de publicación/cola que RFID.

**Entregable:** una caja con QR también genera evento y llega a Supabase, usando la misma tubería que RFID.

## Fase 5 — Integración física (≈20h)
- Montaje del punto de lectura (soporte, antena, cámara, alimentación).
- Ajuste de rango de lectura RFID real y encuadre de cámara para QR.
- Prueba de paso real de una caja por la estación.

**Entregable:** estación de lectura física operativa con ambos sensores.

## Fase 6 — Validación funcional (≈30h)
- Batería de pruebas: exactitud de lectura, tolerancia a desconexión, consistencia de datos, viabilidad de consulta posterior (son los criterios que la propia memoria fija como objetivo específico).
- Registro de métricas/resultados para la sección de resultados y discusión.

**Entregable:** tabla de resultados de pruebas, lista para pegar en la memoria.

## Fase 7 — Documentación (≈60h)
- Diseño detallado (arquitectura, diagramas, modelo de datos) en la memoria.
- Cronograma real (este plan, ajustado con fechas/horas reales invertidas).
- Presupuesto orientativo, resultados, discusión, conclusiones.

## Buffer (≈20h)
Imprevistos de hardware (piezas que no llegan, lector defectuoso, etc.) y ajustes de última hora.

**Total estimado: ≈270h**, dejando margen dentro del límite de 300h (parte ya consumida en antecedentes/objetivos).

---

## Fuera del alcance núcleo (futuro)
- **Fase 8 — Frontend web** (`frontend/`): visualización de eventos, historial por caja, dashboard. Solo si sobra tiempo tras la Fase 7; no cuenta en el presupuesto de horas anterior.

## Próximo paso inmediato
Fase 0: montar el proyecto PlatformIO y el proyecto Supabase.
