import os
from pathlib import Path

try:
    from dotenv import load_dotenv
    root_dir = Path(__file__).resolve().parent.parent
    env_path = root_dir / ".env"
    if env_path.exists():
        load_dotenv(env_path)
    else:
        load_dotenv()
except ImportError:
    pass

ESP32_IP = os.getenv("ESP32_IP", "10.242.93.137")
MQTT_BROKER = os.getenv("MQTT_BROKER", ESP32_IP)
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
SECRET_KEY = os.getenv("SECRET_KEY", "safecell_ai_super_secret_jwt_key_2026")
JWT_ALGORITHM = os.getenv("JWT_ALGORITHM", "HS256")
HOST = os.getenv("HOST", "0.0.0.0")
PORT = int(os.getenv("PORT", "8000"))
