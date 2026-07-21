from sqlalchemy import BigInteger, Column, DateTime, ForeignKey, String, func

from app.db.session import Base


class Event(Base):
    __tablename__ = "events"

    id = Column(BigInteger, primary_key=True)
    box_id = Column(BigInteger, ForeignKey("boxes.id"), nullable=True)
    read_type = Column(String, nullable=False)  # 'RFID' | 'QR'
    raw_value = Column(String, nullable=False)
    read_point = Column(String, nullable=True)
    read_at = Column(DateTime(timezone=True), nullable=False)
    received_at = Column(DateTime(timezone=True), server_default=func.now(), nullable=False)
