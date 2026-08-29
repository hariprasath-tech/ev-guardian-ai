# SafeCell AI — Architectural Decisions & Specifications

## 1. Pre-Implementation Decisions

### 1.1 Login/Account Auth Stub (Rule 7)
Login/Account authentication is **out of scope** for this prototype build.
- The `LoginScreen` is built as a stub interface.
- App startup flow: Welcome/Onboarding → Home Dashboard directly.
- No remote auth, user database, or token management is implemented.

### 1.2 UI Theme (Section 4.5)
The application enforces a **light theme** using flat surfaces, card-based layouts, and clear semantic status indicators.
- Page Background: `#EDEDEA`
- Screen Surface: `#F7F7F5`
- Card Surface: `#FFFFFF` (border `#E5E5E1`, 0.5px, no drop shadow)
- Primary Text: `#17181A`
- Secondary Text: `#8A8D91`
- Semantic Colors:
  - SAFE: `#97C459` on `#EAF3DE`
  - WARNING: `#EF9F27` on `#FAEEDA`
  - CRITICAL: `#E24B4A` on `#FCEBEB`

### 1.3 CAN Bus Integration (Phase 10)
Phase 10 is **SKIPPED**.
- No documented CAN DBC / frame specifications for the target EV/BMS were provided.
- Per Rule 0.1, CAN IDs and signal scaling are not fabricated.
- The firmware includes `firmware/can/canBus.h` as a documented stub.

### 1.4 Cloud Scope (Rule 6)
Wi-Fi, MQTT, FastAPI backend, and PostgreSQL database are explicitly out of scope for this build. Telemetry and alert models maintain 1:1 field compatibility with Section 4.6 backend reference contract.

---

## 2. Hardware Configuration (Confirmed)

- **Target MCU**: ESP32-WROOM-32
- **BLE Library**: NimBLE-Arduino (firmware) / react-native-ble-plx (mobile app)
- **Pin Mapping (ADC1 Only)**:
  - GPIO 4: DS18B20 Temp Sensor (OneWire)
  - GPIO 34: MQ-7 Gas Sensor (CO)
  - GPIO 35: MQ-8 Gas Sensor (H2)
  - GPIO 32: Flame Sensor (Digital)
  - GPIO 33: Flame Sensor (Analog)
  - GPIO 36: Voltage Divider Module (0-25V)
  - GPIO 39: ACS712 Current Sensor
