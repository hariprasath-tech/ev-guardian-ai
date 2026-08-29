/**
 * SafeCell AI — Telemetry Data Structures (Firmware)
 *
 * Mirror: safecell-app/src/models/telemetry.ts
 * Field names, types, and units are IDENTICAL on both sides.
 *
 * If any field is added/renamed/removed here, update telemetry.ts in the app.
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ─── Enumerations ─────────────────────────────────────────────────────────────

typedef enum {
    VEHICLE_STATUS_SAFE     = 0,
    VEHICLE_STATUS_WARNING  = 1,
    VEHICLE_STATUS_CRITICAL = 2,
    VEHICLE_STATUS_INVALID  = 3
} VehicleStatus_t;

typedef enum {
    CHARGING_STATUS_NOT_CHARGING = 0,
    CHARGING_STATUS_CHARGING     = 1,
    CHARGING_STATUS_FAULT        = 2
} ChargingStatus_t;

typedef enum {
    ALERT_SEVERITY_WARNING  = 0,
    ALERT_SEVERITY_CRITICAL = 1
} AlertSeverity_t;

// ─── Fault Record ─────────────────────────────────────────────────────────────

#define FAULT_CODE_MAX_LEN      24
#define FAULT_MESSAGE_MAX_LEN   64
#define FAULT_CHANNEL_MAX_LEN   16
#define MAX_ACTIVE_FAULTS        8

typedef struct {
    char           code[FAULT_CODE_MAX_LEN];        // e.g. "TEMP_HIGH"
    char           message[FAULT_MESSAGE_MAX_LEN];   // Human-readable
    AlertSeverity_t severity;
    uint32_t       timestamp;                        // UTC epoch seconds
    char           sensorChannel[FAULT_CHANNEL_MAX_LEN]; // e.g. "temperature"
} FaultRecord_t;

// ─── Sensor Readings (raw, before validation) ─────────────────────────────────

typedef struct {
    float    temperature_c;      // °C — DS18B20
    float    gas_mq7_ppm;        // CO ppm — MQ-7
    float    gas_mq8_ppm;        // H2 ppm — MQ-8
    float    voltage_v;          // V — voltage divider module
    float    current_a;          // A — ACS712 (positive = discharge)
    bool     fire_detected;      // true = flame module triggered
    uint16_t flame_intensity;    // 0–4095 raw ADC (analog flame channel)
    bool     temp_valid;
    bool     gas_mq7_valid;
    bool     gas_mq8_valid;
    bool     voltage_valid;
    bool     current_valid;
    bool     fire_valid;
} SensorReadings_t;

// ─── Telemetry Packet ─────────────────────────────────────────────────────────

typedef struct {
    char              deviceId[32];
    uint32_t          timestamp;         // UTC epoch seconds
    uint32_t          sequence;          // Monotonically increasing
    float             temperature;       // °C
    float             gasLevel;          // CO ppm (MQ-7, primary gas metric)
    float             gasLevelH2;        // H2 ppm (MQ-8, secondary gas metric)
    float             voltage;           // V
    float             current;           // A
    float             batteryPercentage; // %
    bool              fireDetected;
    uint16_t          flameIntensity;    // 0–4095 ADC raw
    ChargingStatus_t  chargingStatus;
    VehicleStatus_t   vehicleStatus;
    FaultRecord_t     faults[MAX_ACTIVE_FAULTS];
    uint8_t           faultCount;
} TelemetryPacket_t;

// ─── Function Declarations ────────────────────────────────────────────────────

size_t telemetry_serialize(const TelemetryPacket_t* packet, char* outBuf, size_t bufSize);
void telemetry_init(TelemetryPacket_t* packet, const char* deviceId);
void telemetry_increment_sequence(TelemetryPacket_t* packet);
bool telemetry_add_fault(TelemetryPacket_t* packet, const char* code, const char* message, AlertSeverity_t severity, uint32_t timestamp, const char* channel);
void telemetry_clear_faults(TelemetryPacket_t* packet);
