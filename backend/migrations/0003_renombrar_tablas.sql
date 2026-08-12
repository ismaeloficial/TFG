-- Renombra las tablas para reflejar mejor su significado real:
-- "boxes" -> "fichas_caja": la ficha de alta de cada caja (en un despliegue real
--   la rellenaría un departamento de etiquetado/registro; aquí se hace a mano).
-- "events" -> "historial": el histórico de lecturas RFID/QR.

ALTER TABLE boxes RENAME TO fichas_caja;
ALTER TABLE events RENAME TO historial;
ALTER TABLE historial RENAME COLUMN box_id TO ficha_caja_id;
