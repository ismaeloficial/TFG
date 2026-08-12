from datetime import datetime
from typing import Optional

from pydantic import BaseModel, ConfigDict, field_validator

READ_TYPES = ("RFID", "QR")


class HistorialIn(BaseModel):
    """Payload que publica el firmware por MQTT."""

    raw_value: str
    read_type: str
    read_at: datetime
    read_point: Optional[str] = None

    @field_validator("read_type")
    @classmethod
    def validate_read_type(cls, value: str) -> str:
        if value not in READ_TYPES:
            raise ValueError(f"read_type debe ser uno de {READ_TYPES}")
        return value

    @field_validator("raw_value")
    @classmethod
    def validate_raw_value(cls, value: str) -> str:
        if not value.strip():
            raise ValueError("raw_value no puede estar vacío")
        return value


class HistorialOut(BaseModel):
    """Respuesta de la API para una entrada de historial ya persistida."""

    model_config = ConfigDict(from_attributes=True)

    id: int
    ficha_caja_id: Optional[int]
    read_type: str
    raw_value: str
    read_point: Optional[str]
    read_at: datetime
    received_at: datetime
