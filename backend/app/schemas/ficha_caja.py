from datetime import date, datetime
from decimal import Decimal
from typing import Optional

from pydantic import BaseModel, ConfigDict, field_validator


class FichaCajaIn(BaseModel):
    """Payload para dar de alta una ficha de caja."""

    code: str
    tipo_fruta: str
    variedad: Optional[str] = None
    lote: Optional[str] = None
    fecha_recoleccion: Optional[date] = None
    procedencia: Optional[str] = None
    peso_kg: Optional[Decimal] = None

    @field_validator("code", "tipo_fruta")
    @classmethod
    def not_blank(cls, value: str) -> str:
        if not value.strip():
            raise ValueError("no puede estar vacío")
        return value.strip()


class FichaCajaOut(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int
    code: str
    tipo_fruta: str
    variedad: Optional[str]
    lote: Optional[str]
    fecha_recoleccion: Optional[date]
    procedencia: Optional[str]
    peso_kg: Optional[Decimal]
    created_at: datetime
