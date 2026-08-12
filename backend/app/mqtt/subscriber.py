import logging

import paho.mqtt.client as mqtt
from pydantic import ValidationError

from app.core.config import settings
from app.db.session import SessionLocal
from app.models.ficha_caja import FichaCaja
from app.models.historial import Historial
from app.schemas.historial import HistorialIn

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
        payload = HistorialIn.model_validate_json(msg.payload)
    except ValidationError as exc:
        logger.warning("Payload inválido en topic '%s': %s | body=%r", msg.topic, exc, msg.payload)
        return

    db = SessionLocal()
    try:
        ficha = db.query(FichaCaja).filter(FichaCaja.code == payload.raw_value).one_or_none()
        entrada = Historial(
            ficha_caja_id=ficha.id if ficha else None,
            read_type=payload.read_type,
            raw_value=payload.raw_value,
            read_point=payload.read_point,
            read_at=payload.read_at,
        )
        db.add(entrada)
        db.commit()
        logger.info(
            "Historial guardado: %s '%s' (ficha_caja_id=%s, read_point=%s)",
            payload.read_type,
            payload.raw_value,
            entrada.ficha_caja_id,
            payload.read_point,
        )
    except Exception:
        db.rollback()
        logger.exception("Error guardando entrada de historial MQTT en la base de datos")
    finally:
        db.close()


def create_mqtt_client() -> mqtt.Client:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="trazabilidad-backend")
    client.on_connect = _on_connect
    client.on_disconnect = _on_disconnect
    client.on_message = _on_message
    # Por defecto paho-mqtt usa backoff exponencial (1s, 2s, 4s...) hasta 120s.
    # Si el broker cae varias veces seguidas, el reintento puede quedar "dormido"
    # bastante tiempo y perderse mensajes QoS 0 publicados justo cuando el broker
    # ya está de vuelta pero el backend todavía no se ha reenganchado. Con un
    # backoff corto y acotado se reduce mucho esa ventana.
    client.reconnect_delay_set(min_delay=1, max_delay=5)
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
