# SafeCell AI — Cold-Start Demo & Bench Setup Instructions

## 1. Bench Hardware Setup

### Hardware Checklist
- **MCU**: ESP32-WROOM-32 Development Board
- **Temperature Sensor**: DS18B20 1-Wire Digital Temp Probe + 4.7kΩ pull-up resistor (DQ → GPIO 4)
- **CO Gas Sensor**: MQ-7 Module (AOUT → GPIO 34 via 5V→3.3V divider; VCC = 5V)
- **H2 Gas Sensor**: MQ-8 Module (AOUT → GPIO 35 via 5V→3.3V divider; VCC = 5V)
- **Flame Sensor**: IR Flame Sensor Module (DOUT → GPIO 32; AOUT → GPIO 33)
- **Voltage Sensor**: 0–25V Voltage Divider Module (Signal → GPIO 36)
- **Current Sensor**: ACS712 Current Sensor Module (OUT → GPIO 39 via 5V→3.3V divider; VCC = 5V)
- **Target Mobile**: Android Physical Device

---

## 2. Firmware Flashing

1. Connect ESP32 via USB.
2. Open `safecell-firmware/` in VS Code / PlatformIO.
3. Run PlatformIO build and flash:
   ```bash
   pio run --target upload
   ```
4. Monitor serial logs at 115200 baud:
   ```bash
   pio device monitor
   ```
   *Expected Output*:
   ```
   ========================================
    SafeCell AI — ESP32 Firmware
     Version:   0.1.0
     Device ID: SAFE_CELL_001
     BLE Name:  SafeCell EV
   ========================================
   [INIT] Sensors          ... OK
   [INIT] BLE              ... OK
   [BLE] Server initialized. Advertising as 'SafeCell EV'
   ```

---

## 3. Mobile App Deployment

1. Navigate to `safecell-app/`:
   ```bash
   cd safecell-app
   ```
2. Launch on physical Android device:
   ```bash
   npx expo run:android
   ```
   *(Or start dev client with `npx expo start --dev-client`)*

---

## 4. Cold-Start Demonstration Walkthrough

1. **Power On ESP32**: Observe serial logging confirming BLE GATT server initialization and advertising as `SafeCell EV`.
2. **Open Mobile App**: The app launches directly into Welcome/Onboarding → Home Dashboard.
3. **Connect to Gateway**:
   - Tap the top connection pill (`SIMULATOR` / `SCANNING`).
   - Confirm scan discovers `SafeCell EV`.
   - Connected status updates to `BLE LIVE` with active green dot.
4. **Observe Live Telemetry**:
   - Circular status ring shows `SAFE` with green fill and checkmark.
   - Temperature, Voltage, Current, Gas, and Battery SOC update at ~1 Hz cadence.
5. **Simulate / Trigger Fault Conditions**:
   - In Simulator mode: Tap `WARNING`, `CRITICAL`, `STALE`, or `INVALID` scenario pills on the Hero Card.
   - On Physical Sensors:
     - Heat DS18B20 probe above 45°C → Observe `WARNING` state & alert card.
     - Expose MQ-7 to CO gas above 200 ppm or trigger flame sensor → Observe `CRITICAL` state, red ring, and emergency banner.
6. **Test Emergency Shortcut**:
   - Tap the red Emergency Banner on the dashboard or navigate to Safety tab → `Emergency Protocol`.
   - Verify 3-step emergency procedure and direct gateway fault list.
7. **Test Telemetry Freshness**:
   - Power down ESP32 mid-stream or select `STALE` scenario.
   - Observe freshness state transition to `STALE` after 3 seconds.
   - Verify stale data is clearly marked and never presented as live.
