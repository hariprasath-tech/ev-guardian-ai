/**
 * SafeCell AI — CAN Integration
 *
 * STATUS: SKIPPED
 *
 * Reason: No documented CAN interface is available for the target EV/BMS.
 * Per build Rule 0.1, CAN IDs, signal definitions, byte order, scaling, and
 * offsets MUST NOT be fabricated from assumptions.
 *
 * To enable Phase 10:
 *   1. Obtain the target vehicle's CAN DBC file or equivalent signal documentation
 *      from the manufacturer's service manual or a verified source.
 *   2. Populate canSignalConfigs below with real, documented values.
 *   3. Replace this stub with a real implementation using the ESP32 TWAI driver.
 *
 * See DECISIONS.md for the full rationale.
 */

#pragma once

// ─── PLACEHOLDER: Populate from real vehicle documentation ────────────────────
// typedef struct {
//     uint32_t frame_id;       // PLACEHOLDER: e.g. 0x1B0 for some BMS
//     uint8_t  byte_offset;    // PLACEHOLDER: byte position in the frame
//     uint8_t  bit_length;     // PLACEHOLDER
//     float    scale;          // PLACEHOLDER: e.g. 0.1 for tenths of a unit
//     float    offset;         // PLACEHOLDER
//     char     unit[8];        // PLACEHOLDER: e.g. "V", "A", "°C"
//     char     signal_name[24];// PLACEHOLDER
// } CanSignalConfig_t;

/**
 * Stub: returns false — CAN not available.
 * Replace with real TWAI init when Phase 10 is activated.
 */
static inline bool can_init(void) { return false; }
