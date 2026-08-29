/**
 * SafeCell AI — BLE GATT Server Implementation
 *
 * Uses NimBLE-Arduino (h2zero) for GATT server.
 *
 * Characteristics:
 *   - BatteryState, Temperature, Voltage, Current, GasLevel, FireStatus,
 *     VehicleStatus, FaultAlerts: Read/Notify — ESP32 notifies on every
 *     telemetry cycle.
 *   - Command: Write-only — strict allow-list, all unknown commands rejected.
 *
 * Each Read/Notify characteristic carries a compact JSON payload:
 *   { "value": <number/boolean/string>, "ts": <epoch_s>, "seq": <uint32> }
 * The FaultAlerts characteristic carries the full faults JSON array.
 * The VehicleStatus characteristic also carries the full telemetry JSON.
 */

#include "bleServer.h"
#include "bleConfig.h"
#include "config/config.h"
#include <NimBLEDevice.h>
#include <Arduino.h>
#include <string.h>

// ─── Globals ──────────────────────────────────────────────────────────────────
static NimBLEServer*          s_pServer          = nullptr;
static NimBLEService*         s_pService         = nullptr;
static NimBLECharacteristic*  s_pCharBattery     = nullptr;
static NimBLECharacteristic*  s_pCharTemp        = nullptr;
static NimBLECharacteristic*  s_pCharVoltage     = nullptr;
static NimBLECharacteristic*  s_pCharCurrent     = nullptr;
static NimBLECharacteristic*  s_pCharGas         = nullptr;
static NimBLECharacteristic*  s_pCharFire        = nullptr;
static NimBLECharacteristic*  s_pCharStatus      = nullptr;
static NimBLECharacteristic*  s_pCharFaults      = nullptr;
static NimBLECharacteristic*  s_pCharCommand     = nullptr;

static volatile uint8_t       s_connected_count  = 0;

// ─── Server Callbacks ─────────────────────────────────────────────────────────

class SafeCellServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) override {
        s_connected_count++;
        Serial.printf("[BLE] Client connected. Conn handle: %d | Total: %d\n",
                      desc->conn_handle, s_connected_count);
        // Allow more connections (up to 3)
        if (s_connected_count < 3) {
            pServer->startAdvertising();
        }
    }

    void onDisconnect(NimBLEServer* pServer) override {
        if (s_connected_count > 0) s_connected_count--;
        Serial.printf("[BLE] Client disconnected. Total: %d\n", s_connected_count);
        pServer->startAdvertising();
        Serial.println("[BLE] Advertising restarted");
    }
};

// ─── Command Characteristic Callbacks ────────────────────────────────────────

class CommandCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pChar) override {
        std::string rawValue = pChar->getValue();
        const char* cmd = rawValue.c_str();

        // Allow-list check
        bool allowed = (strcmp(cmd, CMD_REQUEST_STATUS) == 0) ||
                       (strcmp(cmd, CMD_RESET_FAULTS)   == 0) ||
                       (strcmp(cmd, CMD_PING)            == 0);

        if (!allowed) {
            Serial.printf("[BLE] REJECTED unknown command: '%s'\n", cmd);
            return;
        }

        Serial.printf("[BLE] Command received: '%s'\n", cmd);

        if (strcmp(cmd, CMD_PING) == 0) {
            // Respond on the command characteristic itself
            pChar->setValue("PONG");
            pChar->notify();
        }
        // CMD_REQUEST_STATUS and CMD_RESET_FAULTS are handled in the main task
        // by checking a shared flag (future enhancement — stubs here for Phase 8)
    }
};

// ─── Initialization ───────────────────────────────────────────────────────────

void ble_server_init(void) {
    NimBLEDevice::init(BLE_DEVICE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Maximum TX power

    s_pServer = NimBLEDevice::createServer();
    s_pServer->setCallbacks(new SafeCellServerCallbacks());

    s_pService = s_pServer->createService(BLE_SERVICE_UUID);

    // Helper lambda: create a Read+Notify characteristic
    auto makeNotify = [&](const char* uuid) -> NimBLECharacteristic* {
        return s_pService->createCharacteristic(
            uuid,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
    };

    s_pCharBattery = makeNotify(BLE_CHAR_BATTERY_STATE_UUID);
    s_pCharTemp    = makeNotify(BLE_CHAR_TEMPERATURE_UUID);
    s_pCharVoltage = makeNotify(BLE_CHAR_VOLTAGE_UUID);
    s_pCharCurrent = makeNotify(BLE_CHAR_CURRENT_UUID);
    s_pCharGas     = makeNotify(BLE_CHAR_GAS_LEVEL_UUID);
    s_pCharFire    = makeNotify(BLE_CHAR_FIRE_STATUS_UUID);
    s_pCharStatus  = makeNotify(BLE_CHAR_VEHICLE_STATUS_UUID);
    s_pCharFaults  = makeNotify(BLE_CHAR_FAULT_ALERTS_UUID);

    // Command: Write only
    s_pCharCommand = s_pService->createCharacteristic(
        BLE_CHAR_COMMAND_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    s_pCharCommand->setCallbacks(new CommandCallbacks());

    s_pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    NimBLEDevice::startAdvertising();

    Serial.printf("[BLE] Server initialized. Advertising as '%s'\n", BLE_DEVICE_NAME);
    Serial.printf("[BLE] Service UUID: %s\n", BLE_SERVICE_UUID);
}

// ─── Notify All Characteristics ───────────────────────────────────────────────

void ble_server_notify(const TelemetryPacket_t* packet, const char* json) {
    if (s_connected_count == 0) return;

    char buf[64];

    // Battery State
    snprintf(buf, sizeof(buf), "{\"value\":%.1f,\"ts\":%lu,\"seq\":%lu}",
             packet->batteryPercentage, packet->timestamp, packet->sequence);
    s_pCharBattery->setValue(buf);
    s_pCharBattery->notify();

    // Temperature
    snprintf(buf, sizeof(buf), "{\"value\":%.2f,\"ts\":%lu,\"seq\":%lu}",
             packet->temperature, packet->timestamp, packet->sequence);
    s_pCharTemp->setValue(buf);
    s_pCharTemp->notify();

    // Voltage
    snprintf(buf, sizeof(buf), "{\"value\":%.2f,\"ts\":%lu,\"seq\":%lu}",
             packet->voltage, packet->timestamp, packet->sequence);
    s_pCharVoltage->setValue(buf);
    s_pCharVoltage->notify();

    // Current
    snprintf(buf, sizeof(buf), "{\"value\":%.2f,\"ts\":%lu,\"seq\":%lu}",
             packet->current, packet->timestamp, packet->sequence);
    s_pCharCurrent->setValue(buf);
    s_pCharCurrent->notify();

    // Gas Level (MQ-7 CO, primary)
    snprintf(buf, sizeof(buf), "{\"value\":%.1f,\"ts\":%lu,\"seq\":%lu}",
             packet->gasLevel, packet->timestamp, packet->sequence);
    s_pCharGas->setValue(buf);
    s_pCharGas->notify();

    // Fire Status
    snprintf(buf, sizeof(buf), "{\"value\":%s,\"intensity\":%d,\"ts\":%lu,\"seq\":%lu}",
             packet->fireDetected ? "true" : "false",
             packet->flameIntensity,
             packet->timestamp, packet->sequence);
    s_pCharFire->setValue(buf);
    s_pCharFire->notify();

    // Vehicle Status — carry full telemetry JSON
    s_pCharStatus->setValue(json);
    s_pCharStatus->notify();

    // Fault Alerts — JSON array of active faults (from full packet JSON)
    // The app can also get faults from the full Vehicle Status notification
    s_pCharFaults->setValue(json);
    s_pCharFaults->notify();
}

bool ble_server_is_connected(void) {
    return s_connected_count > 0;
}

uint8_t ble_server_connected_count(void) {
    return s_connected_count;
}
