from sqlalchemy import BigInteger, Column, DateTime, ForeignKey, String, func

from app.db.session import Base


class Historial(Base):
    __tablename__ = "historial"

    id = Column(BigInteger, primary_key=True)
    ficha_caja_id = Column(BigInteger, ForeignKey("fichas_caja.id"), nullable=True)
    read_type = Column(String, nullable=False)  # 'RFID' | 'QR'
    raw_value = Column(String, nullable=False)
    read_point = Column(String, nullable=True)
    read_at = Column(DateTime(timezone=True), nullable=False)
    received_at = Column(DateTime(timezone=True), server_default=func.now(), nullable=False)
