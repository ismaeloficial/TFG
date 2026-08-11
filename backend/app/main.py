import logging
from contextlib import asynccontextmanager

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy import text

from app.api.boxes import router as boxes_router
from app.api.events import router as events_router
from app.db.session import engine
from app.mqtt.subscriber import start_mqtt_subscriber, stop_mqtt_subscriber

logging.basicConfig(level=logging.INFO)


@asynccontextmanager
async def lifespan(app: FastAPI):
    mqtt_client = start_mqtt_subscriber()
    app.state.mqtt_client = mqtt_client
    yield
    stop_mqtt_subscriber(mqtt_client)


app = FastAPI(title="Trazabilidad de cajas de fruta - Backend", lifespan=lifespan)

# Dev local: frontend (Vite) corre en otro puerto/origen que el backend.
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5175"],
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(events_router)
app.include_router(boxes_router)


@app.get("/health")
def health():
    with engine.connect() as conn:
        conn.execute(text("SELECT 1"))
    return {"status": "ok", "db": "connected"}
