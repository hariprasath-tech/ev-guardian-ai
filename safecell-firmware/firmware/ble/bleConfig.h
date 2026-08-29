/**
 * SafeCell AI — BLE GATT UUID Definitions (Firmware)
 *
 * FROZEN in Phase 2 — do NOT change after this point.
 * Mirror: safecell-app/src/config/bleUUIDs.ts (identical values)
 *
 * These UUIDs are random 128-bit values generated for this project.
 * They are NOT standard Bluetooth SIG UUIDs.
 */

#pragma once

// ─── Service ──────────────────────────────────────────────────────────────────
#define BLE_SERVICE_UUID                "4FAFC201-1FB5-459E-8FCC-C5C9C3319141"

// ─── Characteristics (ESP32 → App, Read/Notify) ───────────────────────────────
#define BLE_CHAR_BATTERY_STATE_UUID     "BEB5483E-36E1-4688-B7F5-EA07361B26A8"
#define BLE_CHAR_TEMPERATURE_UUID       "BEB5483E-36E1-4688-B7F5-EA07361B26A9"
#define BLE_CHAR_VOLTAGE_UUID           "BEB5483E-36E1-4688-B7F5-EA07361B26AA"
#define BLE_CHAR_CURRENT_UUID           "BEB5483E-36E1-4688-B7F5-EA07361B26AB"
#define BLE_CHAR_GAS_LEVEL_UUID         "BEB5483E-36E1-4688-B7F5-EA07361B26AC"
#define BLE_CHAR_FIRE_STATUS_UUID       "BEB5483E-36E1-4688-B7F5-EA07361B26AD"
#define BLE_CHAR_VEHICLE_STATUS_UUID    "BEB5483E-36E1-4688-B7F5-EA07361B26AE"
#define BLE_CHAR_FAULT_ALERTS_UUID      "BEB5483E-36E1-4688-B7F5-EA07361B26AF"

// ─── Command Characteristic (App → ESP32, Write only) ─────────────────────────
#define BLE_CHAR_COMMAND_UUID           "BEB5483E-36E1-4688-B7F5-EA07361B26B0"

// ─── Allowed Command Strings (allow-list — reject everything else) ─────────────
#define CMD_REQUEST_STATUS      "STATUS"
#define CMD_RESET_FAULTS        "RESET_FAULTS"
#define CMD_PING                "PING"
