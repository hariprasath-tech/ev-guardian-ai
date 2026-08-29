/**
 * SafeCell AI — Telemetry Serialization
 *
 * Serializes TelemetryPacket_t to JSON using ArduinoJson.
 * The JSON field names are identical to those in safecell-app/src/models/telemetry.ts.
 */

#include "telemetry.h"
#include <ArduinoJson.h>
#include <string.h>
#include <stdio.h>

// Convert VehicleStatus enum to JSON string
static const char* vehicleStatusToString(VehicleStatus_t status) {
    switch (status) {
        case VEHICLE_STATUS_SAFE:     return "SAFE";
        case VEHICLE_STATUS_WARNING:  return "WARNING";
        case VEHICLE_STATUS_CRITICAL: return "CRITICAL";
        case VEHICLE_STATUS_INVALID:
        default:                       return "INVALID";
    }
}

// Convert ChargingStatus enum to JSON string
static const char* chargingStatusToString(ChargingStatus_t status) {
    switch (status) {
        case CHARGING_STATUS_CHARGING:     return "CHARGING";
        case CHARGING_STATUS_NOT_CHARGING: return "NOT_CHARGING";
        case CHARGING_STATUS_FAULT:
        default:                            return "FAULT";
    }
}

// Convert AlertSeverity enum to JSON string
static const char* severityToString(AlertSeverity_t severity) {
    switch (severity) {
        case ALERT_SEVERITY_CRITICAL: return "CRITICAL";
        case ALERT_SEVERITY_WARNING:
        default:                       return "WARNING";
    }
}

/**
 * Serialize a TelemetryPacket_t to a JSON string.
 *
 * @param packet   Pointer to the telemetry packet to serialize.
 * @param outBuf   Buffer to write the JSON string into.
 * @param bufSize  Size of outBuf.
 * @return         Number of bytes written (excluding null terminator), or 0 on error.
 */
size_t telemetry_serialize(const TelemetryPacket_t* packet, char* outBuf, size_t bufSize) {
    // Allocate a JSON document. Size: base fields (~300 bytes) + faults (up to 8 * ~150 bytes)
    JsonDocument doc;

    doc["deviceId"]          = packet->deviceId;
    doc["timestamp"]         = packet->timestamp;
    doc["sequence"]          = packet->sequence;
    doc["temperature"]       = serialized(String(packet->temperature, 2));
    doc["gasLevel"]          = serialized(String(packet->gasLevel, 1));
    doc["gasLevelH2"]        = serialized(String(packet->gasLevelH2, 1));
    doc["voltage"]           = serialized(String(packet->voltage, 2));
    doc["current"]           = serialized(String(packet->current, 2));
    doc["batteryPercentage"] = serialized(String(packet->batteryPercentage, 1));
    doc["fireDetected"]      = packet->fireDetected;
    doc["flameIntensity"]    = packet->flameIntensity;
    doc["chargingStatus"]    = chargingStatusToString(packet->chargingStatus);
    doc["vehicleStatus"]     = vehicleStatusToString(packet->vehicleStatus);

    JsonArray faultsArray = doc["faults"].to<JsonArray>();
    for (uint8_t i = 0; i < packet->faultCount && i < MAX_ACTIVE_FAULTS; i++) {
        JsonObject fault = faultsArray.add<JsonObject>();
        fault["code"]          = packet->faults[i].code;
        fault["message"]       = packet->faults[i].message;
        fault["severity"]      = severityToString(packet->faults[i].severity);
        fault["timestamp"]     = packet->faults[i].timestamp;
        fault["sensorChannel"] = packet->faults[i].sensorChannel;
    }

    size_t written = serializeJson(doc, outBuf, bufSize);
    return written;
}

/**
 * Initialize a TelemetryPacket_t with safe defaults.
 */
void telemetry_init(TelemetryPacket_t* packet, const char* deviceId) {
    memset(packet, 0, sizeof(TelemetryPacket_t));
    strncpy(packet->deviceId, deviceId, sizeof(packet->deviceId) - 1);
    packet->vehicleStatus  = VEHICLE_STATUS_INVALID;
    packet->chargingStatus = CHARGING_STATUS_NOT_CHARGING;
    packet->sequence       = 0;
}

/**
 * Increment the sequence counter (wraps at UINT32_MAX → 0).
 */
void telemetry_increment_sequence(TelemetryPacket_t* packet) {
    packet->sequence++;
}

/**
 * Add a fault record to the packet.
 * Returns false if the fault array is full (MAX_ACTIVE_FAULTS).
 */
bool telemetry_add_fault(TelemetryPacket_t* packet, const char* code,
                          const char* message, AlertSeverity_t severity,
                          uint32_t timestamp, const char* channel) {
    if (packet->faultCount >= MAX_ACTIVE_FAULTS) {
        return false;
    }
    FaultRecord_t* f = &packet->faults[packet->faultCount];
    strncpy(f->code,          code,    sizeof(f->code) - 1);
    strncpy(f->message,       message, sizeof(f->message) - 1);
    strncpy(f->sensorChannel, channel, sizeof(f->sensorChannel) - 1);
    f->severity  = severity;
    f->timestamp = timestamp;
    packet->faultCount++;
    return true;
}

/**
 * Clear all fault records.
 */
void telemetry_clear_faults(TelemetryPacket_t* packet) {
    packet->faultCount = 0;
    memset(packet->faults, 0, sizeof(packet->faults));
}
