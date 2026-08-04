import time
import random
from backend.ai_engine import AIBatteryEngine
from backend.database import save_telemetry

class HardwareAbstractionLayer:
    def __init__(self):
        self.mode = "prototype"  # "prototype" or "hardware"
        self.device_id = "SC-ESP32-04A2"
        self.mac_address = "A4:CF:12:8B:3E:9D"
        self.firmware_version = "v2.3.1"
        self.system_armed = True
        self.sensitivity = 0.60
        self.emergency_active = False

        # Live telemetry state cache
        self.latest_telemetry = {
            "device_id": self.device_id,
            "mac_address": self.mac_address,
            "firmware_version": self.firmware_version,
            "battery_temp": 31.0,
            "battery_voltage": 398.4,
            "battery_current": 12.5,
            "co2_ppm": 412.0,
            "smoke_density": 0.02,
            "thermal_gradient": 6.0,
            "runaway_risk": 1.8,
            "battery_health": 98.0,
            "cycle_count": 342,
            "latitude": 37.7749,
            "longitude": -122.4194,
            "system_armed": self.system_armed,
            "sensitivity": self.sensitivity,
            "emergency_active": self.emergency_active,
            "mode": self.mode,
            "timestamp": time.time()
        }

    def set_mode(self, mode: str):
        if mode in ["prototype", "hardware"]:
            self.mode = mode
            self.latest_telemetry["mode"] = mode
            return True
        return False

    def update_hardware_telemetry(self, data: dict):
        """Called when real ESP32 transmits telemetry in Hardware Mode."""
        self.latest_telemetry.update(data)
        self.latest_telemetry["mode"] = self.mode

        # Run AI analysis
        ai_res = AIBatteryEngine.analyze_telemetry(
            self.latest_telemetry["battery_temp"],
            self.latest_telemetry["battery_voltage"],
            self.latest_telemetry["battery_current"],
            self.latest_telemetry["co2_ppm"],
            self.latest_telemetry["smoke_density"]
        )
        self.latest_telemetry.update(ai_res)
        self.latest_telemetry["timestamp"] = time.time()

        # Save to DB
        save_telemetry(self.latest_telemetry)

    def generate_prototype_telemetry(self):
        """Generates realistic telemetry stream when in Prototype Mode."""
        if self.mode != "prototype":
            return self.latest_telemetry

        temp = round(31.0 + random.uniform(-0.3, 0.3), 1)
        voltage = round(398.4 + random.uniform(-0.2, 0.2), 1)
        current = round(12.5 + random.uniform(-0.5, 0.5), 1)
        co2 = round(412.0 + random.uniform(-3, 3), 1)
        smoke = round(max(0.0, 0.02 + random.uniform(-0.003, 0.003)), 3)

        ai_res = AIBatteryEngine.analyze_telemetry(temp, voltage, current, co2, smoke)

        self.latest_telemetry.update({
            "device_id": self.device_id,
            "mac_address": self.mac_address,
            "firmware_version": self.firmware_version,
            "battery_temp": temp,
            "battery_voltage": voltage,
            "battery_current": current,
            "co2_ppm": co2,
            "smoke_density": smoke,
            "cycle_count": 342,
            "latitude": 37.7749,
            "longitude": -122.4194,
            "system_armed": self.system_armed,
            "sensitivity": self.sensitivity,
            "emergency_active": self.emergency_active,
            "mode": self.mode,
            "timestamp": time.time()
        })
        self.latest_telemetry.update(ai_res)

        save_telemetry(self.latest_telemetry)
        return self.latest_telemetry

hal = HardwareAbstractionLayer()
