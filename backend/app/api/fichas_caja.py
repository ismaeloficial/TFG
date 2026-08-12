from typing import List

from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import Session

from app.db.session import get_db
from app.models.ficha_caja import FichaCaja
from app.schemas.ficha_caja import FichaCajaIn, FichaCajaOut

router = APIRouter(prefix="/fichas-caja", tags=["fichas-caja"])


@router.get("", response_model=List[FichaCajaOut])
def list_fichas_caja(db: Session = Depends(get_db)):
    return db.query(FichaCaja).order_by(FichaCaja.created_at.desc()).all()


@router.post("", response_model=FichaCajaOut, status_code=201)
def create_ficha_caja(payload: FichaCajaIn, db: Session = Depends(get_db)):
    ficha = FichaCaja(**payload.model_dump())
    db.add(ficha)
    try:
        db.commit()
    except IntegrityError:
        db.rollback()
        raise HTTPException(status_code=409, detail=f"Ya existe una ficha con code '{payload.code}'")
    db.refresh(ficha)
    return ficha


@router.put("/{ficha_id}", response_model=FichaCajaOut)
def update_ficha_caja(ficha_id: int, payload: FichaCajaIn, db: Session = Depends(get_db)):
    ficha = db.query(FichaCaja).filter(FichaCaja.id == ficha_id).one_or_none()
    if ficha is None:
        raise HTTPException(status_code=404, detail="Ficha no encontrada")

    for field, value in payload.model_dump().items():
        setattr(ficha, field, value)

    try:
        db.commit()
    except IntegrityError:
        db.rollback()
        raise HTTPException(status_code=409, detail=f"Ya existe otra ficha con code '{payload.code}'")
    db.refresh(ficha)
    return ficha
