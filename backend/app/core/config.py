from pydantic_settings import BaseSettings


class Settings(BaseSettings):
    supabase_db_url: str
    mqtt_broker_host: str = "localhost"
    mqtt_broker_port: int = 1883
    mqtt_topic_events: str = "trazabilidad/eventos"

    class Config:
        env_file = ".env"


settings = Settings()
