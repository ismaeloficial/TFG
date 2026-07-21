from sqlalchemy import BigInteger, Column, DateTime, String, func

from app.db.session import Base


class Box(Base):
    __tablename__ = "boxes"

    id = Column(BigInteger, primary_key=True)
    code = Column(String, nullable=False, unique=True)
    created_at = Column(DateTime(timezone=True), server_default=func.now(), nullable=False)
