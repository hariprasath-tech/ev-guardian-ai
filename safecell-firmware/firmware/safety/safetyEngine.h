/**
 * SafeCell AI — Safety Engine (Header)
 *
 * Evaluates VehicleStatus from sensor readings using threshold constants.
 * Runs INDEPENDENTLY of the mobile phone — the ESP32 is the safety authority.
 *
 * IMPORTANT: Safety decisions are never made from INVALID readings.
 * If a required sensor is invalid, the output for that channel is INVALID,
 * not SAFE/WARNING/CRITICAL.
 */

#pragma once
#include "telemetry/telemetry.h"

/**
 * Evaluate overall vehicle safety status from a full set of sensor readings.
 * Populates the vehicleStatus field and any triggered faults into the packet.
 *
 * Rules:
 *   - Any CRITICAL channel → overall CRITICAL
 *   - Any WARNING channel (no CRITICAL) → overall WARNING
 *   - All channels SAFE and valid → overall SAFE
 *   - Any required channel INVALID and the rest are SAFE → overall WARNING
 *     (degraded sensing is a warning condition, not SAFE)
 *
 * @param readings  Sensor readings to evaluate (must be freshly read)
 * @param packet    Output packet to populate vehicleStatus and faults into
 * @param timestamp Current UTC epoch seconds (for fault timestamps)
 */
void safety_evaluate(const SensorReadings_t* readings,
                     TelemetryPacket_t*      packet,
                     uint32_t               timestamp);
