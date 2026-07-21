-- Esquema inicial: cajas y eventos de lectura (RFID/QR)

CREATE TABLE IF NOT EXISTS boxes (
    id BIGSERIAL PRIMARY KEY,
    code TEXT NOT NULL UNIQUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS events (
    id BIGSERIAL PRIMARY KEY,
    box_id BIGINT REFERENCES boxes(id),
    read_type TEXT NOT NULL CHECK (read_type IN ('RFID', 'QR')),
    raw_value TEXT NOT NULL,
    read_point TEXT,
    read_at TIMESTAMPTZ NOT NULL,
    received_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS idx_events_box_id ON events(box_id);
CREATE INDEX IF NOT EXISTS idx_events_read_at ON events(read_at);
