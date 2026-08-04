# SafeCell AI — ESP32 Hardware & Arduino IDE Deployment Guide

This folder contains the complete production-grade C++ firmware for running **SafeCell AI** hardware nodes on an **ESP32 / ESP32-S3** microcontroller board, connecting physical IoT sensors to the FastAPI backend and Next.js Web Application.

---

## 🔌 Hardware Pinout Wiring Guide

| Component | Sensor / Actuator Type | ESP32 Pin | Signal Type | Description |
| :--- | :--- | :--- | :--- | :--- |
| **DHT22 / DHT11** | Temperature & Humidity | `GPIO 4` | Digital Input | Battery Pack Ambient Temp (°C) & Humidity |
| **MQ-7 Gas** | Carbon Monoxide / CO₂ | `GPIO 33` | Analog ADC | Cabin & Pack Air Quality (PPM) |
| **MQ-8 Smoke** | Optical Smoke / Particulate | `GPIO 32` | Analog ADC | Smoke Density Percentage (%) |
| **Flame Sensor** | Optical IR Fire Detector | `GPIO 35` | Digital Input | Immediate Flame Detection (LOW = Fire) |
| **Voltage Divider** | Pack Voltage Step-Down | `GPIO 34` | Analog ADC | Battery Voltage (0 - 500V DC scale) |
| **ACS712 Current** | Hall Effect Current Shunt | `GPIO 36` (VP) | Analog ADC | Pack Current (0 - 30A DC scale) |
| **5V Relay Module**| Aerosol Extinguisher Actuator| `GPIO 26` | Digital Output | HIGH = Discharge Fire Suppression Aerosol |
| **Active Buzzer** | Emergency Warning Siren | `GPIO 27` | Digital Output | HIGH = Loud Alarm Sound |
| **Status LED** | Onboard Blue Indicator | `GPIO 2` | Digital Output | Solid = WebSockets Connected, Blink = Setup |

---

## 💻 Arduino IDE Setup Guide (Step-by-Step)

### Step 1: Install Arduino IDE
Download and install [Arduino IDE 2.x](https://www.arduino.cc/en/software) or Arduino IDE 1.8.x.

### Step 2: Add ESP32 Board Manager URL
1. Open Arduino IDE and go to **File** $\rightarrow$ **Preferences**.
2. In **Additional Boards Manager URLs**, paste:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Click **OK**.
4. Go to **Tools** $\rightarrow$ **Board** $\rightarrow$ **Boards Manager**.
5. Search for `esp32` by Espressif Systems and click **Install**.

### Step 3: Install Required Libraries
Go to **Tools** $\rightarrow$ **Manage Libraries...** (or `Ctrl+Shift+I`) and install the following 5 libraries:

1. **`ArduinoJson`** by *Benoit Blanchon* (Version 6.21+ or 7.x)
2. **`WebSockets`** by *Markus Sattler*
3. **`PubSubClient`** by *Nick O'Leary*
4. **`DHT sensor library`** by *Adafruit*
5. **`Adafruit Unified Sensor`** by *Adafruit*

---

## ⚡ Flashing the Firmware to your ESP32

1. Open `hardware/esp32_firmware.ino` in Arduino IDE.
2. Edit lines 24–30 with your Wi-Fi credentials and Laptop Local IP address:
   ```cpp
   const char* WIFI_SSID     = "YOUR_WIFI_SSID";     // Your Wi-Fi network name
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Your Wi-Fi password
   const char* BACKEND_HOST  = "192.168.1.100";      // Your laptop's local IP address (find via ipconfig / ifconfig)
   ```
3. Connect your ESP32 board to your laptop via USB cable.
4. Under **Tools** $\rightarrow$ **Board**, select **`ESP32 Dev Module`** (or **`ESP32-S3 Dev Module`**).
5. Under **Tools** $\rightarrow$ **Port**, select your ESP32 COM Port (e.g., `COM3`, `COM4`, `/dev/ttyUSB0`).
6. Click the **Upload** arrow button (or press `Ctrl+U`).
7. Open **Tools** $\rightarrow$ **Serial Monitor** and set the baud rate to **`115200`**.

---

## 📡 Live Verification & Debug Output

Once uploaded, the ESP32 Serial Monitor will output real-time diagnostic logs:

```text
========================================================
 SafeCell AI — Production ESP32 Hardware Firmware
========================================================
[WIFI] Connecting to SSID: YourWiFiNetwork
[WIFI] Connected Successfully!
[WIFI] IP Address : 192.168.1.105
[WIFI] MAC Address: A4:CF:12:8B:3E:9D
[WS] WebSocket Stream Connected Successfully!
[TELEMETRY] Temp: 31.5°C | CO2: 412 ppm | Smoke: 0.02% | Volt: 398.4V | Risk: 1.8%
```

---

## 🧪 Testing Without Physical Hardware

If you don't have physical ESP32 hardware attached right now, run the included Python telemetry hardware simulator from your terminal:

```bash
python hardware/esp32_simulator.py
```
This streams realistic battery and cabin telemetry over WebSockets to your dashboard automatically!
