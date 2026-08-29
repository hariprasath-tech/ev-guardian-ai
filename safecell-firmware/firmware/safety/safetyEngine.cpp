/**
 * SafeCell AI — Safety Engine Implementation
 *
 * Per-channel threshold evaluation. Each channel returns one of:
 *   SAFE / WARNING / CRITICAL / INVALID
 *
 * INVALID is returned when the sensor reading is out of valid range.
 * An INVALID reading MUST NOT produce a SAFE output — it produces WARNING.
 *
 * Overall status: CRITICAL > WARNING > (INVALID treated as WARNING) > SAFE
 */

#include "safetyEngine.h"
#include "config/thresholds.h"
#include <string.h>
#include <Arduino.h>

// ─── Per-Channel Evaluators ───────────────────────────────────────────────────

static VehicleStatus_t eval_temperature(const SensorReadings_t* r,
                                         TelemetryPacket_t* packet,
                                         uint32_t ts) {
    if (!r->temp_valid) {
        telemetry_add_fault(packet, "TEMP_INVALID",
            "Temperature sensor reading out of valid range or disconnected",
            ALERT_SEVERITY_WARNING, ts, "temperature");
        return VEHICLE_STATUS_INVALID;
    }
    if (r->temperature_c >= TEMP_CRITICAL_C) {
        telemetry_add_fault(packet, "TEMP_CRITICAL",
            "Battery temperature critical — exceeds maximum safe operating limit",
            ALERT_SEVERITY_CRITICAL, ts, "temperature");
        return VEHICLE_STATUS_CRITICAL;
    }
    if (r->temperature_c >= TEMP_WARNING_C) {
        telemetry_add_fault(packet, "TEMP_WARNING",
            "Battery temperature elevated — approaching operating limit",
            ALERT_SEVERITY_WARNING, ts, "temperature");
        return VEHICLE_STATUS_WARNING;
    }
    return VEHICLE_STATUS_SAFE;
}

static VehicleStatus_t eval_mq7(const SensorReadings_t* r,
                                  TelemetryPacket_t* packet,
                                  uint32_t ts) {
    if (!r->gas_mq7_valid) {
        telemetry_add_fault(packet, "GAS_CO_INVALID",
            "CO sensor (MQ-7) reading invalid or out of range",
            ALERT_SEVERITY_WARNING, ts, "gasLevel");
        return VEHICLE_STATUS_INVALID;
    }
    if (r->gas_mq7_ppm >= MQ7_PPM_CRITICAL) {
        telemetry_add_fault(packet, "GAS_CO_CRITICAL",
            "Carbon monoxide level critically high — immediate ventilation required",
            ALERT_SEVERITY_CRITICAL, ts, "gasLevel");
        return VEHICLE_STATUS_CRITICAL;
    }
    if (r->gas_mq7_ppm >= MQ7_PPM_WARNING) {
        telemetry_add_fault(packet, "GAS_CO_WARNING",
            "Carbon monoxide level elevated — check vehicle ventilation",
            ALERT_SEVERITY_WARNING, ts, "gasLevel");
        return VEHICLE_STATUS_WARNING;
    }
    return VEHICLE_STATUS_SAFE;
}

static VehicleStatus_t eval_mq8(const SensorReadings_t* r,
                                  TelemetryPacket_t* packet,
                                  uint32_t ts) {
    if (!r->gas_mq8_valid) {
        telemetry_add_fault(packet, "GAS_H2_INVALID",
            "Hydrogen sensor (MQ-8) reading invalid or out of range",
            ALERT_SEVERITY_WARNING, ts, "gasLevelH2");
        return VEHICLE_STATUS_INVALID;
    }
    if (r->gas_mq8_ppm >= MQ8_PPM_CRITICAL) {
        telemetry_add_fault(packet, "GAS_H2_CRITICAL",
            "Hydrogen level critically high — fire/explosion risk",
            ALERT_SEVERITY_CRITICAL, ts, "gasLevelH2");
        return VEHICLE_STATUS_CRITICAL;
    }
    if (r->gas_mq8_ppm >= MQ8_PPM_WARNING) {
        telemetry_add_fault(packet, "GAS_H2_WARNING",
            "Hydrogen level elevated — monitor closely",
            ALERT_SEVERITY_WARNING, ts, "gasLevelH2");
        return VEHICLE_STATUS_WARNING;
    }
    return VEHICLE_STATUS_SAFE;
}

static VehicleStatus_t eval_flame(const SensorReadings_t* r,
                                   TelemetryPacket_t* packet,
                                   uint32_t ts) {
    if (!r->fire_valid) {
        telemetry_add_fault(packet, "FIRE_SENSOR_INVALID",
            "Flame sensor invalid",
            ALERT_SEVERITY_WARNING, ts, "fireDetected");
        return VEHICLE_STATUS_INVALID;
    }
    if (r->fire_detected) {
        telemetry_add_fault(packet, "FIRE_DETECTED",
            "Flame detected — CRITICAL fire hazard",
            ALERT_SEVERITY_CRITICAL, ts, "fireDetected");
        return VEHICLE_STATUS_CRITICAL;
    }
    return VEHICLE_STATUS_SAFE;
}

static VehicleStatus_t eval_voltage(const SensorReadings_t* r,
                                     TelemetryPacket_t* packet,
                                     uint32_t ts) {
    if (!r->voltage_valid) {
        telemetry_add_fault(packet, "VOLT_INVALID",
            "Voltage reading out of valid range",
            ALERT_SEVERITY_WARNING, ts, "voltage");
        return VEHICLE_STATUS_INVALID;
    }
    if (r->voltage_v <= VOLT_CRITICAL_LOW_V || r->voltage_v >= VOLT_CRITICAL_HIGH_V) {
        telemetry_add_fault(packet, "VOLT_CRITICAL",
            "Battery voltage critical — outside safe operating bounds",
            ALERT_SEVERITY_CRITICAL, ts, "voltage");
        return VEHICLE_STATUS_CRITICAL;
    }
    if (r->voltage_v <= VOLT_WARNING_LOW_V || r->voltage_v >= VOLT_WARNING_HIGH_V) {
        telemetry_add_fault(packet, "VOLT_WARNING",
            "Battery voltage approaching limits",
            ALERT_SEVERITY_WARNING, ts, "voltage");
        return VEHICLE_STATUS_WARNING;
    }
    return VEHICLE_STATUS_SAFE;
}

static VehicleStatus_t eval_current(const SensorReadings_t* r,
                                     TelemetryPacket_t* packet,
                                     uint32_t ts) {
    if (!r->current_valid) {
        telemetry_add_fault(packet, "CURRENT_INVALID",
            "Current reading out of valid range",
            ALERT_SEVERITY_WARNING, ts, "current");
        return VEHICLE_STATUS_INVALID;
    }
    float abs_current = r->current_a < 0 ? -r->current_a : r->current_a;
    if (abs_current >= CURRENT_CRITICAL_A) {
        telemetry_add_fault(packet, "CURRENT_CRITICAL",
            "Battery current critically high — potential overcurrent condition",
            ALERT_SEVERITY_CRITICAL, ts, "current");
        return VEHICLE_STATUS_CRITICAL;
    }
    if (abs_current >= CURRENT_WARNING_A) {
        telemetry_add_fault(packet, "CURRENT_WARNING",
            "Battery current elevated",
            ALERT_SEVERITY_WARNING, ts, "current");
        return VEHICLE_STATUS_WARNING;
    }
    return VEHICLE_STATUS_SAFE;
}

// ─── Overall Evaluator ────────────────────────────────────────────────────────

void safety_evaluate(const SensorReadings_t* readings,
                     TelemetryPacket_t*      packet,
                     uint32_t               timestamp) {
    telemetry_clear_faults(packet);

    VehicleStatus_t channel_results[6];
    channel_results[0] = eval_temperature(readings, packet, timestamp);
    channel_results[1] = eval_mq7(readings, packet, timestamp);
    channel_results[2] = eval_mq8(readings, packet, timestamp);
    channel_results[3] = eval_flame(readings, packet, timestamp);
    channel_results[4] = eval_voltage(readings, packet, timestamp);
    channel_results[5] = eval_current(readings, packet, timestamp);

    VehicleStatus_t overall = VEHICLE_STATUS_SAFE;
    for (int i = 0; i < 6; i++) {
        if (channel_results[i] == VEHICLE_STATUS_CRITICAL) {
            overall = VEHICLE_STATUS_CRITICAL;
            break; // CRITICAL is the highest priority; no need to check further
        }
        if (channel_results[i] == VEHICLE_STATUS_WARNING ||
            channel_results[i] == VEHICLE_STATUS_INVALID) {
            // INVALID treated as WARNING at the overall level
            overall = VEHICLE_STATUS_WARNING;
        }
    }

    packet->vehicleStatus = overall;

    Serial.printf("[SAFETY] Status: %s | Faults: %d\n",
        overall == VEHICLE_STATUS_SAFE     ? "SAFE"     :
        overall == VEHICLE_STATUS_WARNING  ? "WARNING"  :
        overall == VEHICLE_STATUS_CRITICAL ? "CRITICAL" : "INVALID",
        packet->faultCount);
}
