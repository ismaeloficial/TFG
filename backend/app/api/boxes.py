from typing import List

from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import Session

from app.db.session import get_db
from app.models.box import Box
from app.schemas.box import BoxIn, BoxOut

router = APIRouter(prefix="/boxes", tags=["boxes"])


@router.get("", response_model=List[BoxOut])
def list_boxes(db: Session = Depends(get_db)):
    return db.query(Box).order_by(Box.created_at.desc()).all()


@router.post("", response_model=BoxOut, status_code=201)
def create_box(payload: BoxIn, db: Session = Depends(get_db)):
    box = Box(**payload.model_dump())
    db.add(box)
    try:
        db.commit()
    except IntegrityError:
        db.rollback()
        raise HTTPException(status_code=409, detail=f"Ya existe una caja con code '{payload.code}'")
    db.refresh(box)
    return box


@router.put("/{box_id}", response_model=BoxOut)
def update_box(box_id: int, payload: BoxIn, db: Session = Depends(get_db)):
    box = db.query(Box).filter(Box.id == box_id).one_or_none()
    if box is None:
        raise HTTPException(status_code=404, detail="Caja no encontrada")

    for field, value in payload.model_dump().items():
        setattr(box, field, value)

    try:
        db.commit()
    except IntegrityError:
        db.rollback()
        raise HTTPException(status_code=409, detail=f"Ya existe otra caja con code '{payload.code}'")
    db.refresh(box)
    return box
