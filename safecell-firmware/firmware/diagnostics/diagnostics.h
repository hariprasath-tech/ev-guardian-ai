/**
 * SafeCell AI — Diagnostics
 *
 * Boot-time hardware verification and runtime diagnostic logging.
 */

#pragma once
#include <stdint.h>

/**
 * Log firmware version, device ID, and hardware info at boot.
 * Call first in setup() before any other init.
 */
void diagnostics_boot_banner(void);

/**
 * Log the result of a hardware init step.
 * @param component  Name of the component (e.g. "BLE", "Sensors")
 * @param success    true = initialized OK, false = failed
 */
void diagnostics_init_result(const char* component, bool success);

/**
 * Log a runtime diagnostic message with a component prefix.
 */
void diagnostics_log(const char* component, const char* message);

/**
 * Log free heap memory — useful for tracking memory leaks over time.
 */
void diagnostics_log_heap(void);
