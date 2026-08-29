/**
 * SafeCell AI — BLE GATT Server (Header)
 *
 * NimBLE-Arduino based GATT server implementing the SafeCell EV Service.
 * All characteristic UUIDs are defined in bleConfig.h (frozen in Phase 2).
 *
 * The server handles:
 *   - Advertising as "SafeCell EV"
 *   - 9 characteristics per the data contract (Section 3.2)
 *   - Notifying all subscribed clients every TELEMETRY_INTERVAL_MS
 *   - Command write-only characteristic with strict allow-list
 *   - Connection/disconnection callbacks → updates BLE state
 */

#pragma once
#include "telemetry/telemetry.h"

/**
 * Initialize NimBLE, create the GATT service and all characteristics.
 * Must be called once from setup().
 */
void ble_server_init(void);

/**
 * Notify all subscribed BLE clients with the latest telemetry packet.
 * Called from the telemetry task every TELEMETRY_INTERVAL_MS.
 *
 * @param packet The current telemetry data to broadcast.
 * @param json   Pre-serialized JSON string of the full telemetry packet.
 */
void ble_server_notify(const TelemetryPacket_t* packet, const char* json);

/**
 * Returns true if at least one BLE client is currently connected.
 */
bool ble_server_is_connected(void);

/**
 * Returns the number of currently connected BLE clients.
 */
uint8_t ble_server_connected_count(void);
