#!/usr/bin/env python3
"""
SafeCell AI — ESP32 Telemetry Hardware Simulator
Simulates real-time EV Battery & Fire Suppression telemetry sending data to the SafeCell Backend over WebSockets & REST API.
"""

import asyncio
import json
import random
import time
import sys
try:
    import websockets
except ImportError:
    print("Installing websockets library...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "websockets"])
    import websockets

BACKEND_WS_URL = "ws://127.0.0.1:8000/ws/telemetry"

class ESP32Simulator:
    def __init__(self, device_id="SC-ESP32-04A2", mac_address="A4:CF:12:8B:3E:9D"):
        self.device_id = device_id
        self.mac_address = mac_address
        self.firmware_version = "v2.3.1"
        self.battery_temp = 31.0
        self.battery_voltage = 398.4
        self.battery_current = 12.5
        self.co2_ppm = 412.0
        self.smoke_density = 0.02
        self.battery_health = 98.0
        self.cycle_count = 342
        self.latitude = 37.7749
        self.longitude = -122.4194
        self.system_armed = True
        self.sensitivity = 0.60
        self.emergency_active = False
        self.running = True

    def generate_telemetry(self):
        # Apply realistic noise and minor drift
        self.battery_temp += random.uniform(-0.2, 0.2)
        self.battery_temp = max(20.0, min(85.0, round(self.battery_temp, 1)))

        self.battery_voltage += random.uniform(-0.1, 0.1)
        self.battery_voltage = round(max(350.0, min(420.0, self.battery_voltage)), 1)

        self.battery_current += random.uniform(-0.3, 0.3)
        self.battery_current = round(max(0.0, min(150.0, self.battery_current)), 1)

        self.co2_ppm += random.uniform(-2, 2)
        self.co2_ppm = round(max(380.0, min(2000.0, self.co2_ppm)), 1)

        self.smoke_density += random.uniform(-0.005, 0.005)
        self.smoke_density = round(max(0.0, min(1.0, self.smoke_density)), 3)

        # AI thermal runaway risk calculation
        thermal_gradient = round((self.battery_temp - 25.0) * 1.5 + (self.smoke_density * 200), 1)
        runaway_risk = round(min(100.0, max(0.0, (self.battery_temp - 30) * 1.8 + self.smoke_density * 300)), 1)

        return {
            "device_id": self.device_id,
            "mac_address": self.mac_address,
            "firmware_version": self.firmware_version,
            "battery_temp": self.battery_temp,
            "battery_voltage": self.battery_voltage,
            "battery_current": self.battery_current,
            "co2_ppm": self.co2_ppm,
            "smoke_density": self.smoke_density,
            "thermal_gradient": thermal_gradient,
            "runaway_risk": runaway_risk,
            "battery_health": self.battery_health,
            "cycle_count": self.cycle_count,
            "latitude": self.latitude,
            "longitude": self.longitude,
            "system_armed": self.system_armed,
            "sensitivity": self.sensitivity,
            "emergency_active": self.emergency_active,
            "timestamp": time.time()
        }

    async def start(self):
        print(f"🚀 Starting ESP32 Simulator Node [{self.device_id}] ({self.mac_address})")
        while self.running:
            try:
                print(f"Connecting to Backend WebSocket: {BACKEND_WS_URL}...")
                async with websockets.connect(BACKEND_WS_URL) as ws:
                    print("✅ Connected to SafeCell Backend!")
                    while self.running:
                        telemetry = self.generate_telemetry()
                        payload = json.dumps(telemetry)
                        await ws.send(payload)
                        print(f"📡 Sent telemetry: Temp={telemetry['battery_temp']}°C | Voltage={telemetry['battery_voltage']}V | CO2={telemetry['co2_ppm']}ppm | Risk={telemetry['runaway_risk']}%")
                        await asyncio.sleep(1.0)
            except (websockets.exceptions.ConnectionClosedError, ConnectionRefusedError, OSError) as e:
                print(f"⚠️ Connection error: {e}. Retrying in 3 seconds...")
                await asyncio.sleep(3.0)

if __name__ == "__main__":
    simulator = ESP32Simulator()
    try:
        asyncio.run(simulator.start())
    except KeyboardInterrupt:
        print("\nStopping ESP32 Simulator.")
