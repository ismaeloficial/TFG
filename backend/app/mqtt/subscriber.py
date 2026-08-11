import logging

import paho.mqtt.client as mqtt
from pydantic import ValidationError

from app.core.config import settings
from app.db.session import SessionLocal
from app.models.box import Box
from app.models.event import Event
from app.schemas.event import EventIn

logger = logging.getLogger("app.mqtt")


def _on_connect(client, userdata, flags, reason_code, properties=None):
    if reason_code != 0:
        logger.error("Fallo al conectar al broker MQTT: %s", reason_code)
        return
    logger.info("Conectado al broker MQTT, suscribiendo a '%s'", settings.mqtt_topic_events)
    client.subscribe(settings.mqtt_topic_events)


def _on_disconnect(client, userdata, flags, reason_code, properties=None):
    logger.warning("Desconectado del broker MQTT (reason_code=%s)", reason_code)


def _on_message(client, userdata, msg):
    try:
        payload = EventIn.model_validate_json(msg.payload)
    except ValidationError as exc:
        logger.warning("Payload inválido en topic '%s': %s | body=%r", msg.topic, exc, msg.payload)
        return

    db = SessionLocal()
    try:
        box = db.query(Box).filter(Box.code == payload.raw_value).one_or_none()
        event = Event(
            box_id=box.id if box else None,
            read_type=payload.read_type,
            raw_value=payload.raw_value,
            read_point=payload.read_point,
            read_at=payload.read_at,
        )
        db.add(event)
        db.commit()
        logger.info(
            "Evento guardado: %s '%s' (box_id=%s, read_point=%s)",
            payload.read_type,
            payload.raw_value,
            event.box_id,
            payload.read_point,
        )
    except Exception:
        db.rollback()
        logger.exception("Error guardando evento MQTT en la base de datos")
    finally:
        db.close()


def create_mqtt_client() -> mqtt.Client:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="trazabilidad-backend")
    client.on_connect = _on_connect
    client.on_disconnect = _on_disconnect
    client.on_message = _on_message
    return client


def start_mqtt_subscriber() -> mqtt.Client:
    """Conecta al broker y arranca el loop de red en un hilo aparte. Llamar al iniciar la app."""
    client = create_mqtt_client()
    client.connect(settings.mqtt_broker_host, settings.mqtt_broker_port)
    client.loop_start()
    return client


def stop_mqtt_subscriber(client: mqtt.Client) -> None:
    client.loop_stop()
    client.disconnect()
