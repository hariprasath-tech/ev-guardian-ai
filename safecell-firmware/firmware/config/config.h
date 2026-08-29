/**
 * SafeCell AI — Firmware Configuration
 *
 * All thresholds, pin assignments, and calibration constants that need to be
 * set per hardware installation are in separate files:
 *   - config/pins.h        — GPIO assignments
 *   - config/thresholds.h  — safety threshold values
 *   - config/calibration.h — sensor calibration constants
 *
 * Do NOT hardcode magic numbers outside of config/. Every value that has a
 * real-world meaning (pin number, ppm limit, voltage threshold) must be a
 * named constant defined here or in a sibling config header.
 */

#pragma once

// ─── Device Identity ─────────────────────────────────────────────────────────
#define DEVICE_ID           "SAFE_CELL_001"
#define FIRMWARE_VERSION    "0.1.0"
#define BLE_DEVICE_NAME     "SafeCell EV"

// ─── Telemetry Timing ─────────────────────────────────────────────────────────
// Must match TELEMETRY_INTERVAL_MS in safecell-app/src/config/constants.ts
#define TELEMETRY_INTERVAL_MS       1000U   // 1 second telemetry cadence
#define SENSOR_SAMPLE_COUNT         20U     // ADC oversampling count per reading
#define SENSOR_WARMUP_MS            20000U  // MQ sensor warmup period (20s minimum)

// ─── Serial Debug ─────────────────────────────────────────────────────────────
#define SERIAL_BAUD_RATE    115200

// ─── FreeRTOS Task Stack Sizes (bytes) ────────────────────────────────────────
#define TASK_STACK_SENSOR       4096U
#define TASK_STACK_TELEMETRY    4096U
#define TASK_STACK_BLE          8192U

// ─── FreeRTOS Task Priorities ─────────────────────────────────────────────────
#define TASK_PRIORITY_SENSOR        2
#define TASK_PRIORITY_TELEMETRY     2
#define TASK_PRIORITY_BLE           3     // BLE gets highest priority

// ─── Watchdog ─────────────────────────────────────────────────────────────────
#define WDT_TIMEOUT_S       10U
