import json
import threading
import time

try:
    import paho.mqtt.client as mqtt
    MQTT_AVAILABLE = True
except ImportError:
    MQTT_AVAILABLE = False

from backend.config import ESP32_IP
from backend.hal import hal

class SafeCellMQTTClient:
    def __init__(self, broker=ESP32_IP, port=1883):
        self.broker = broker
        self.port = port
        self.client = None
        self.connected = False

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            self.connected = True
            print(f"✅ SafeCell MQTT Client connected to {self.broker}:{self.port}")
            client.subscribe("safecell/telemetry")
        else:
            print(f"⚠️ MQTT Connection failed with result code {rc}")

    def on_message(self, client, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode())
            print(f"📡 Received MQTT Telemetry: {payload}")
            if hal.mode == "hardware":
                hal.update_hardware_telemetry(payload)
        except Exception as e:
            print(f"Error parsing MQTT message: {e}")

    def start(self):
        if not MQTT_AVAILABLE:
            print("paho-mqtt not installed. MQTT client listener disabled.")
            return

        def run():
            try:
                self.client = mqtt.Client(client_id="SafeCellBackend")
                self.client.on_connect = self.on_connect
                self.client.on_message = self.on_message
                self.client.connect_async(self.broker, self.port, 60)
                self.client.loop_start()
            except Exception as e:
                print(f"Could not connect to MQTT Broker at {self.broker}:{self.port}: {e}")

        t = threading.Thread(target=run, daemon=True)
        t.start()

mqtt_service = SafeCellMQTTClient()
