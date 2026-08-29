/**
 * SafeCell AI — Sensor Interface (Header)
 *
 * Abstracts all physical sensors behind a single SensorReadings_t struct.
 * Phase 5: All functions return SIMULATED data.
 * Phase 9: Replace implementations in sensors.cpp with real hardware reads.
 *
 * Sensors:
 *   - Temperature: DS18B20 (OneWire, GPIO 4)
 *   - Gas CO:      MQ-7 (ADC1_CH6, GPIO 34)
 *   - Gas H2:      MQ-8 (ADC1_CH7, GPIO 35)
 *   - Flame:       Flame Sensor Module (digital GPIO 32, analog GPIO 33)
 *   - Voltage:     0-25V module (ADC1_CH0, GPIO 36)
 *   - Current:     ACS712 (ADC1_CH3, GPIO 39)
 */

#pragma once
#include "telemetry/telemetry.h"

/**
 * Initialize all sensor hardware.
 * Must be called once from setup() before sensor_read_all().
 * Returns true if all sensors initialized successfully.
 */
bool sensors_init(void);

/**
 * Read all sensors and populate the readings struct.
 * Validity flags in SensorReadings_t indicate whether each reading is trustworthy.
 * Invalid readings must NOT be used in safety evaluation.
 *
 * This function performs ADC oversampling (SENSOR_SAMPLE_COUNT samples per channel)
 * to reduce noise. It is non-blocking if called from a dedicated FreeRTOS task.
 */
void sensors_read_all(SensorReadings_t* readings);

/**
 * Check if all sensors have completed their minimum warmup period.
 * MQ sensors require at least 20 seconds of heater-on time before readings stabilize.
 */
bool sensors_warmed_up(void);

/**
 * Estimate battery percentage from voltage.
 * This is a simple linear approximation — replace with actual SOC curve for your
 * battery chemistry and cell count in Phase 9.
 *
 * PLACEHOLDER: Assumes 48V nominal (16S LiFePO4), 40V empty, 58.4V full.
 */
float sensors_estimate_soc(float voltage_v);

/**
 * Determine charging status from current and voltage.
 */
ChargingStatus_t sensors_charging_status(float current_a, float voltage_v);
