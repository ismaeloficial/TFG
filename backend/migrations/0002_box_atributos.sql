-- Atributos de trazabilidad para las cajas (tipo de fruta, lote, procedencia...)

ALTER TABLE boxes
    ADD COLUMN IF NOT EXISTS tipo_fruta TEXT NOT NULL DEFAULT 'sin_especificar',
    ADD COLUMN IF NOT EXISTS variedad TEXT,
    ADD COLUMN IF NOT EXISTS lote TEXT,
    ADD COLUMN IF NOT EXISTS fecha_recoleccion DATE,
    ADD COLUMN IF NOT EXISTS procedencia TEXT,
    ADD COLUMN IF NOT EXISTS peso_kg NUMERIC(6, 2);

-- El DEFAULT de tipo_fruta es solo para no romper filas existentes al migrar;
-- las cajas nuevas siempre deben especificarlo explícitamente (lo exige la API).
ALTER TABLE boxes ALTER COLUMN tipo_fruta DROP DEFAULT;
