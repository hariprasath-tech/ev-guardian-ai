#include "diagnostics.h"
#include "config/config.h"
#include <Arduino.h>
#include <esp_system.h>

void diagnostics_boot_banner(void) {
    Serial.println();
    Serial.println("========================================");
    Serial.println(" SafeCell AI — ESP32 Firmware");
    Serial.printf("  Version:   %s\n", FIRMWARE_VERSION);
    Serial.printf("  Device ID: %s\n", DEVICE_ID);
    Serial.printf("  BLE Name:  %s\n", BLE_DEVICE_NAME);
    Serial.printf("  Chip:      ESP32-WROOM-32\n");
    Serial.printf("  Free heap: %lu bytes\n", (unsigned long)esp_get_free_heap_size());
    Serial.printf("  CPU freq:  %d MHz\n", getCpuFrequencyMhz());
    Serial.println("========================================");
    Serial.println();
}

void diagnostics_init_result(const char* component, bool success) {
    Serial.printf("[INIT] %-16s ... %s\n", component, success ? "OK" : "FAILED");
}

void diagnostics_log(const char* component, const char* message) {
    Serial.printf("[%s] %s\n", component, message);
}

void diagnostics_log_heap(void) {
    Serial.printf("[DIAG] Free heap: %lu bytes\n", (unsigned long)esp_get_free_heap_size());
}
