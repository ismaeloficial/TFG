from typing import List, Optional

from fastapi import APIRouter, Depends, Query
from sqlalchemy.orm import Session

from app.db.session import get_db
from app.models.event import Event
from app.schemas.event import EventOut

router = APIRouter(prefix="/events", tags=["events"])


@router.get("", response_model=List[EventOut])
def list_events(
    box_id: Optional[int] = Query(default=None, description="Filtra por caja"),
    limit: int = Query(default=50, le=200),
    db: Session = Depends(get_db),
):
    """Lista eventos de lectura, más recientes primero. Base para el futuro frontend."""
    query = db.query(Event).order_by(Event.read_at.desc())
    if box_id is not None:
        query = query.filter(Event.box_id == box_id)
    return query.limit(limit).all()
