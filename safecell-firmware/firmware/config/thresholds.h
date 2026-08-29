/**
 * SafeCell AI — Safety Thresholds
 *
 * All thresholds are named constants derived from sensor datasheets and
 * common EV battery safety guidelines. Values marked PLACEHOLDER must be
 * verified and adjusted during Phase 9 calibration with real hardware.
 *
 * Sources:
 *   - MQ-7 datasheet: CO threshold 50 ppm (WHO guideline 8hr), 200 ppm IDLH
 *   - MQ-8 datasheet: H2 LEL is 4% (40,000 ppm), alarm at 1% LEL = ~400 ppm
 *   - DS18B20: valid range -55°C to +125°C
 *   - Generic LiFePO4 battery: charge 3.65V/cell, max temp 60°C
 *
 * References are not certified safety limits — this is a prototype.
 * See Section 2 of the build spec: "never represented as a certified safety system."
 */

#pragma once

// ─── Temperature (°C) ─────────────────────────────────────────────────────────
#define TEMP_VALID_MIN_C        -55.0f   // DS18B20 lower limit
#define TEMP_VALID_MAX_C        125.0f   // DS18B20 upper limit
#define TEMP_SENSOR_DISCONNECT  -127.0f  // DallasTemperature disconnected sentinel
#define TEMP_WARNING_C          45.0f    // PLACEHOLDER: set per battery manufacturer spec
#define TEMP_CRITICAL_C         60.0f    // PLACEHOLDER: set per battery manufacturer spec

// ─── MQ-7 Carbon Monoxide (ppm) ───────────────────────────────────────────────
// Rs/Ro in clean air ≈ 1.0 (MQ-7 datasheet curve intercept)
#define MQ7_RO_CLEAN_AIR_RATIO  1.0f    // Typical; recalibrate in clean air (Phase 9)
#define MQ7_LOAD_RESISTANCE_K   10.0f   // Standard 10kΩ load resistor on MQ modules
#define MQ7_CURVE_A             99.042f  // Power-law curve coefficient a (empirical from datasheet)
#define MQ7_CURVE_B             -1.518f  // Power-law curve exponent b
#define MQ7_PPM_WARNING         50.0f    // WHO 8-hour CO exposure limit (ppm)
#define MQ7_PPM_CRITICAL        200.0f   // IDLH (Immediately Dangerous to Life) CO level

// ─── MQ-8 Hydrogen (ppm) ──────────────────────────────────────────────────────
// Rs/Ro in clean air ≈ 6.5 (MQ-8 datasheet)
#define MQ8_RO_CLEAN_AIR_RATIO  6.5f    // Datasheet value; recalibrate in clean air (Phase 9)
#define MQ8_LOAD_RESISTANCE_K   10.0f   // Standard 10kΩ load resistor
#define MQ8_CURVE_A             2.3f     // Empirical power-law a for H2 (from datasheet curve fit)
#define MQ8_CURVE_B             -0.95f   // Empirical power-law b for H2
#define MQ8_PPM_WARNING         500.0f   // PLACEHOLDER: ~1% of LEL (40,000 ppm) for H2 = 400 ppm, rounded
#define MQ8_PPM_CRITICAL        1000.0f  // PLACEHOLDER: 2.5% LEL for H2

// ─── Voltage (V) ──────────────────────────────────────────────────────────────
// Adjust per target battery pack. Example: 48V 16S LiFePO4 pack.
#define VOLT_VALID_MIN_V        0.0f     // Below this = sensor fault
#define VOLT_VALID_MAX_V        60.0f    // Above this = sensor fault (also ADC range limit)
#define VOLT_WARNING_LOW_V      44.0f    // PLACEHOLDER: set per battery BMS low-voltage spec
#define VOLT_CRITICAL_LOW_V     40.0f    // PLACEHOLDER
#define VOLT_WARNING_HIGH_V     58.8f    // PLACEHOLDER: set per battery BMS over-voltage spec
#define VOLT_CRITICAL_HIGH_V    60.0f    // PLACEHOLDER

// ─── Current (A) ──────────────────────────────────────────────────────────────
// ACS712 variant: 5A (185 mV/A), 20A (100 mV/A), or 30A (66 mV/A)
// Set ACS712_SENSITIVITY_MV_PER_A below to match your exact module variant.
#define ACS712_SENSITIVITY_MV_PER_A  100.0f  // PLACEHOLDER: 100 mV/A for 20A variant
#define ACS712_ZERO_CURRENT_MV      1650.0f  // PLACEHOLDER: ~1.65V after 5V→3.3V divider (2.5V * 3.3/5)
#define CURRENT_VALID_MIN_A         -80.0f   // Negative = regenerative / charging
#define CURRENT_VALID_MAX_A          80.0f
#define CURRENT_WARNING_A            50.0f   // PLACEHOLDER
#define CURRENT_CRITICAL_A           70.0f   // PLACEHOLDER

// ─── Voltage Divider for 0–25V Module ─────────────────────────────────────────
// R1 = 30kΩ, R2 = 7.5kΩ → ratio = (R1+R2)/R2 = 37.5/7.5 = 5.0
#define VOLTAGE_DIVIDER_RATIO       5.0f
#define VOLTAGE_CALIBRATION_FACTOR  1.0f     // PLACEHOLDER: adjust empirically vs multimeter

// ─── ADC Reference ────────────────────────────────────────────────────────────
#define ADC_RESOLUTION_BITS     12
#define ADC_MAX_VALUE           4095.0f
#define ADC_REFERENCE_MV        3300.0f     // ESP32 VREF (approximate — use analogReadMilliVolts for accuracy)
