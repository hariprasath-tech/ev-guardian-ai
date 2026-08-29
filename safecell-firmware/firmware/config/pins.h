/**
 * SafeCell AI — GPIO Pin Assignments
 *
 * Board: ESP32-WROOM-32
 *
 * IMPORTANT: All analog sensor pins are on ADC1 (GPIOs 32–39).
 * ADC2 pins are NOT used because NimBLE BLE stack requires exclusive ADC1 use.
 * ADC2 conflicts with Bluetooth radio on the ESP32.
 *
 * Verify each pin against your actual wiring before Phase 9 testing.
 * These assignments are confirmed per the hardware setup documented in DECISIONS.md.
 */

#pragma once
#include "driver/gpio.h"

// ─── Temperature Sensor (DS18B20, OneWire) ────────────────────────────────────
// Digital I/O — can use any GPIO. 4.7kΩ pull-up resistor required between DQ and 3.3V.
#define PIN_TEMP_ONEWIRE        GPIO_NUM_4

// ─── MQ-7 Carbon Monoxide Gas Sensor ──────────────────────────────────────────
// ADC1_CH6 (input-only, no internal pull-up)
// Sensor AOUT must be stepped down to 0–3.3V via voltage divider (MQ modules output up to 5V).
// Heater VCC must be 5V — do NOT connect heater to ESP32 3.3V rail.
#define PIN_MQ7_ANALOG          GPIO_NUM_34

// ─── MQ-8 Hydrogen Gas Sensor ─────────────────────────────────────────────────
// ADC1_CH7 (input-only, no internal pull-up)
// Same voltage divider requirement as MQ-7.
#define PIN_MQ8_ANALOG          GPIO_NUM_35

// ─── Flame Sensor Module ──────────────────────────────────────────────────────
// Digital output (DOUT): HIGH = no flame, LOW = flame detected (active-low on most modules)
// Analog output (AOUT): ADC1_CH5 — optional, used for flame intensity estimate
#define PIN_FLAME_DIGITAL       GPIO_NUM_32
#define PIN_FLAME_ANALOG        GPIO_NUM_33

// ─── Voltage Sensor Module (0–25V, 5:1 divider) ───────────────────────────────
// ADC1_CH0 / SVP (input-only)
// Module divider ratio: 5:1 (30kΩ / 7.5kΩ). Max safe input: ~16.5V for 3.3V ADC.
// Do NOT connect battery voltages above 16V without adding an extra divider stage.
#define PIN_VOLTAGE_ANALOG      GPIO_NUM_36

// ─── ACS712 Current Sensor ────────────────────────────────────────────────────
// ADC1_CH3 / SVN (input-only)
// ACS712 runs on 5V; its OUT pin must be stepped down to 3.3V via voltage divider.
// Zero-current output: ~2.5V (on 5V supply) → ~1.65V after 5V→3.3V divider.
#define PIN_CURRENT_ANALOG      GPIO_NUM_39
