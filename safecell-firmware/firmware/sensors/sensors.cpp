/**
 * SafeCell AI — Sensor Implementations
 *
 * PHASE 5: All sensor functions return SIMULATED sinusoidal/random data.
 *          The real hardware reads are implemented in Phase 9.
 *
 * PHASE 9: Replace the SIMULATED SECTION in each function with the real
 *          hardware-specific implementation. The function signatures, validity
 *          logic, and struct population remain unchanged.
 *
 * Each sensor function:
 *   1. Reads hardware (or generates simulated data)
 *   2. Validates the reading against range limits from thresholds.h
 *   3. Sets the validity flag in the readings struct
 *   4. Never writes invalid data into the safety-relevant fields
 */

#include "sensors.h"
#include "config/pins.h"
#include "config/thresholds.h"
#include "config/config.h"
#include <Arduino.h>
#include <math.h>

// ─── Simulation State ─────────────────────────────────────────────────────────
// Used by simulated sensor functions. Remove in Phase 9.
static uint32_t s_sim_tick = 0;
static uint32_t s_init_time_ms = 0;

// ─── Phase 9 Hardware Includes ────────────────────────────────────────────────
// Uncomment these in Phase 9 when replacing simulated reads:
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <MQUnifiedsensor.h>
//
// static OneWire           s_oneWire(PIN_TEMP_ONEWIRE);
// static DallasTemperature s_dallasSensors(&s_oneWire);
// static MQUnifiedsensor   s_mq7("ESP32", 3.3f, 12, PIN_MQ7_ANALOG, "MQ-7");
// static MQUnifiedsensor   s_mq8("ESP32", 3.3f, 12, PIN_MQ8_ANALOG, "MQ-8");

// ─── ADC Helper (used in Phase 9) ─────────────────────────────────────────────
// Oversampled ADC read for noise reduction
static uint32_t adc_read_averaged(gpio_num_t pin, uint8_t samples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += analogRead(pin);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    return sum / samples;
}

// ─── Initialization ───────────────────────────────────────────────────────────

bool sensors_init(void) {
    s_init_time_ms = millis();

    // Set ADC attenuation for full 0–3.3V range on all analog pins
    analogSetAttenuation(ADC_11db);

    // Configure flame sensor digital pin
    pinMode(PIN_FLAME_DIGITAL, INPUT);

    // ── PHASE 9: Uncomment hardware init ──────────────────────────────────────
    // s_dallasSensors.begin();
    // if (s_dallasSensors.getDeviceCount() == 0) {
    //     Serial.println("[SENSOR] ERROR: No DS18B20 found on bus");
    //     return false;
    // }
    // s_mq7.setRegressionMethod(1);  // PPM = a*(Rs/Ro)^b
    // s_mq7.setA(MQ7_CURVE_A); s_mq7.setB(MQ7_CURVE_B);
    // s_mq7.init();
    // s_mq7.setRL(MQ7_LOAD_RESISTANCE_K);
    // float mq7_calcR0 = 0;
    // for (int i = 1; i <= 10; i++) {
    //     s_mq7.update();
    //     mq7_calcR0 += s_mq7.calibrate(MQ7_RO_CLEAN_AIR_RATIO);
    // }
    // s_mq7.setR0(mq7_calcR0 / 10);
    // // Repeat for MQ-8
    // s_mq8.setRegressionMethod(1);
    // s_mq8.setA(MQ8_CURVE_A); s_mq8.setB(MQ8_CURVE_B);
    // s_mq8.init();
    // s_mq8.setRL(MQ8_LOAD_RESISTANCE_K);
    // float mq8_calcR0 = 0;
    // for (int i = 1; i <= 10; i++) {
    //     s_mq8.update();
    //     mq8_calcR0 += s_mq8.calibrate(MQ8_RO_CLEAN_AIR_RATIO);
    // }
    // s_mq8.setR0(mq8_calcR0 / 10);
    // ──────────────────────────────────────────────────────────────────────────

    Serial.println("[SENSOR] Initialized (SIMULATED mode — Phase 5)");
    return true;
}

bool sensors_warmed_up(void) {
    return (millis() - s_init_time_ms) >= SENSOR_WARMUP_MS;
}

// ─── Temperature ─────────────────────────────────────────────────────────────

static void read_temperature(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    // Oscillates between 25°C and 50°C with slow drift to test WARNING threshold
    float sim_temp = 35.0f + 15.0f * sinf(s_sim_tick * 0.05f);
    r->temperature_c = sim_temp;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // s_dallasSensors.requestTemperatures();
    // float t = s_dallasSensors.getTempCByIndex(0);
    // r->temperature_c = t;
    // ─────────────────────────────────────────────────────────────────────────

    r->temp_valid = (r->temperature_c > TEMP_VALID_MIN_C) &&
                    (r->temperature_c < TEMP_VALID_MAX_C) &&
                    (r->temperature_c != TEMP_SENSOR_DISCONNECT);
}

// ─── MQ-7 (Carbon Monoxide) ──────────────────────────────────────────────────

static void read_mq7(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    float sim_ppm = 30.0f + 80.0f * (0.5f + 0.5f * sinf(s_sim_tick * 0.03f));
    r->gas_mq7_ppm = sim_ppm;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // s_mq7.update();
    // r->gas_mq7_ppm = s_mq7.readSensor();
    // ─────────────────────────────────────────────────────────────────────────

    r->gas_mq7_valid = (r->gas_mq7_ppm >= 0.0f) && (r->gas_mq7_ppm < 10000.0f);
}

// ─── MQ-8 (Hydrogen) ─────────────────────────────────────────────────────────

static void read_mq8(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    float sim_ppm = 200.0f + 400.0f * (0.5f + 0.5f * sinf(s_sim_tick * 0.04f));
    r->gas_mq8_ppm = sim_ppm;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // s_mq8.update();
    // r->gas_mq8_ppm = s_mq8.readSensor();
    // ─────────────────────────────────────────────────────────────────────────

    r->gas_mq8_valid = (r->gas_mq8_ppm >= 0.0f) && (r->gas_mq8_ppm < 50000.0f);
}

// ─── Flame Sensor ─────────────────────────────────────────────────────────────

static void read_flame(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    // Simulate a brief flame event every ~60 seconds
    bool sim_fire = ((s_sim_tick % 60) < 3);
    r->fire_detected   = sim_fire;
    r->flame_intensity = sim_fire ? 2500 : 50;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // int  digital_val     = digitalRead(PIN_FLAME_DIGITAL);
    // r->fire_detected   = (digital_val == LOW);  // Active LOW on most modules
    // r->flame_intensity = (uint16_t)adc_read_averaged(PIN_FLAME_ANALOG, SENSOR_SAMPLE_COUNT);
    // ─────────────────────────────────────────────────────────────────────────

    r->fire_valid = true; // Flame sensor is always structurally valid
}

// ─── Voltage ─────────────────────────────────────────────────────────────────

static void read_voltage(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    float sim_voltage = 48.0f + 4.0f * sinf(s_sim_tick * 0.02f);
    r->voltage_v = sim_voltage;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // uint32_t raw_mv = 0;
    // for (int i = 0; i < SENSOR_SAMPLE_COUNT; i++) {
    //     raw_mv += analogReadMilliVolts(PIN_VOLTAGE_ANALOG);
    //     vTaskDelay(1 / portTICK_PERIOD_MS);
    // }
    // float vout_v = (raw_mv / SENSOR_SAMPLE_COUNT) / 1000.0f;
    // r->voltage_v = vout_v * VOLTAGE_DIVIDER_RATIO * VOLTAGE_CALIBRATION_FACTOR;
    // ─────────────────────────────────────────────────────────────────────────

    r->voltage_valid = (r->voltage_v >= VOLT_VALID_MIN_V) &&
                       (r->voltage_v <= VOLT_VALID_MAX_V);
}

// ─── Current ─────────────────────────────────────────────────────────────────

static void read_current(SensorReadings_t* r) {
    // ── SIMULATED (Phase 5) ───────────────────────────────────────────────────
    float sim_current = 12.0f + 8.0f * sinf(s_sim_tick * 0.07f);
    r->current_a = sim_current;
    // ── PHASE 9 REPLACEMENT ───────────────────────────────────────────────────
    // uint32_t raw_mv = 0;
    // for (int i = 0; i < SENSOR_SAMPLE_COUNT; i++) {
    //     raw_mv += analogReadMilliVolts(PIN_CURRENT_ANALOG);
    //     vTaskDelay(1 / portTICK_PERIOD_MS);
    // }
    // float vout_mv = (float)(raw_mv / SENSOR_SAMPLE_COUNT);
    // r->current_a  = (vout_mv - ACS712_ZERO_CURRENT_MV) / ACS712_SENSITIVITY_MV_PER_A;
    // ─────────────────────────────────────────────────────────────────────────

    r->current_valid = (r->current_a >= CURRENT_VALID_MIN_A) &&
                       (r->current_a <= CURRENT_VALID_MAX_A);
}

// ─── Main Read ────────────────────────────────────────────────────────────────

void sensors_read_all(SensorReadings_t* readings) {
    s_sim_tick++;
    read_temperature(readings);
    read_mq7(readings);
    read_mq8(readings);
    read_flame(readings);
    read_voltage(readings);
    read_current(readings);
}

// ─── SOC Estimate ─────────────────────────────────────────────────────────────

float sensors_estimate_soc(float voltage_v) {
    // PLACEHOLDER: Linear approximation for 16S LiFePO4 (48V nominal)
    // Replace with real OCV–SOC curve from your battery datasheet in Phase 9
    const float v_empty = 40.0f;
    const float v_full  = 58.4f;
    float soc = (voltage_v - v_empty) / (v_full - v_empty) * 100.0f;
    if (soc < 0.0f)   soc = 0.0f;
    if (soc > 100.0f) soc = 100.0f;
    return soc;
}

ChargingStatus_t sensors_charging_status(float current_a, float voltage_v) {
    // Negative current = current flowing into battery = charging
    if (current_a < -1.0f) return CHARGING_STATUS_CHARGING;
    return CHARGING_STATUS_NOT_CHARGING;
}
