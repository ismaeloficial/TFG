from sqlalchemy import BigInteger, Column, Date, DateTime, Numeric, String, func

from app.db.session import Base


class Box(Base):
    __tablename__ = "boxes"

    id = Column(BigInteger, primary_key=True)
    code = Column(String, nullable=False, unique=True)
    tipo_fruta = Column(String, nullable=False)
    variedad = Column(String, nullable=True)
    lote = Column(String, nullable=True)
    fecha_recoleccion = Column(Date, nullable=True)
    procedencia = Column(String, nullable=True)
    peso_kg = Column(Numeric(6, 2), nullable=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now(), nullable=False)
