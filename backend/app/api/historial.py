from typing import List, Optional

from fastapi import APIRouter, Depends, Query
from sqlalchemy.orm import Session

from app.db.session import get_db
from app.models.historial import Historial
from app.schemas.historial import HistorialOut

router = APIRouter(prefix="/historial", tags=["historial"])


@router.get("", response_model=List[HistorialOut])
def list_historial(
    ficha_caja_id: Optional[int] = Query(default=None, description="Filtra por ficha de caja"),
    limit: int = Query(default=50, le=200),
    db: Session = Depends(get_db),
):
    """Lista el historial de lecturas, más recientes primero."""
    query = db.query(Historial).order_by(Historial.read_at.desc())
    if ficha_caja_id is not None:
        query = query.filter(Historial.ficha_caja_id == ficha_caja_id)
    return query.limit(limit).all()
