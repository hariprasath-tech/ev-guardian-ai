/**
 * SafeCell AI — Main Application Entry Point
 *
 * Architecture:
 *   Core 0 (PRO_CPU): BLE stack (NimBLE) + BLE notify task
 *   Core 1 (APP_CPU): Sensor read task + Telemetry/safety evaluation task
 *
 * Task flow every TELEMETRY_INTERVAL_MS:
 *   SensorTask → reads all sensors → posts to telemetryQueue
 *   TelemetryTask → receives readings → runs safety_evaluate → serializes →
 *                   posts JSON to bleNotifyQueue
 *   BleTask → receives JSON → calls ble_server_notify
 *
 * Watchdog is enabled on both cores.
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_task_wdt.h>
#include <time.h>

#include "config/config.h"
#include "telemetry/telemetry.h"
#include "sensors/sensors.h"
#include "safety/safetyEngine.h"
#include "ble/bleServer.h"
#include "diagnostics/diagnostics.h"

// ─── Shared state ─────────────────────────────────────────────────────────────
static QueueHandle_t s_sensorQueue   = nullptr;  // SensorReadings_t
static QueueHandle_t s_bleNotifyQueue = nullptr; // char[1024] JSON buffer

// JSON buffer size: enough for full telemetry + 8 faults
#define JSON_BUF_SIZE 1024

// Simulated epoch base (real RTC to be added in Phase 9)
// Using a fixed base time + millis() offset as a placeholder timestamp
static const uint32_t EPOCH_BASE = 1720000000UL;

static uint32_t get_timestamp(void) {
    return EPOCH_BASE + (millis() / 1000UL);
}

// ─── Sensor Task (Core 1) ─────────────────────────────────────────────────────

static void sensor_task(void* pvParameters) {
    esp_task_wdt_add(NULL);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    SensorReadings_t readings;

    while (true) {
        esp_task_wdt_reset();
        sensors_read_all(&readings);

        // Overwrite old item if queue is full (always keep latest reading)
        if (xQueueSend(s_sensorQueue, &readings, 0) != pdPASS) {
            SensorReadings_t discard;
            xQueueReceive(s_sensorQueue, &discard, 0);
            xQueueSend(s_sensorQueue, &readings, 0);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TELEMETRY_INTERVAL_MS));
    }
}

// ─── Telemetry + Safety Task (Core 1) ─────────────────────────────────────────

static void telemetry_task(void* pvParameters) {
    esp_task_wdt_add(NULL);

    TelemetryPacket_t packet;
    telemetry_init(&packet, DEVICE_ID);

    SensorReadings_t readings;
    static char jsonBuf[JSON_BUF_SIZE];

    while (true) {
        esp_task_wdt_reset();

        if (xQueueReceive(s_sensorQueue, &readings, pdMS_TO_TICKS(2000)) == pdPASS) {
            uint32_t ts = get_timestamp();
            packet.timestamp = ts;
            telemetry_increment_sequence(&packet);

            // Populate telemetry fields from readings
            packet.temperature       = readings.temperature_c;
            packet.gasLevel          = readings.gas_mq7_ppm;
            packet.gasLevelH2        = readings.gas_mq8_ppm;
            packet.voltage           = readings.voltage_v;
            packet.current           = readings.current_a;
            packet.fireDetected      = readings.fire_detected;
            packet.flameIntensity    = readings.flame_intensity;
            packet.batteryPercentage = sensors_estimate_soc(readings.voltage_v);
            packet.chargingStatus    = sensors_charging_status(readings.current_a,
                                                               readings.voltage_v);

            // Run safety evaluation (modifies vehicleStatus and faults in packet)
            safety_evaluate(&readings, &packet, ts);

            // Serialize to JSON
            size_t written = telemetry_serialize(&packet, jsonBuf, JSON_BUF_SIZE);
            if (written == 0) {
                Serial.println("[TELEMETRY] ERROR: JSON serialization failed");
                continue;
            }

            // Log to serial (Phase 5 Done-When verification)
            Serial.printf("[TELEMETRY] seq=%lu status=%s temp=%.1f°C gas=%.0fppm volt=%.1fV\n",
                packet.sequence,
                packet.vehicleStatus == VEHICLE_STATUS_SAFE     ? "SAFE"     :
                packet.vehicleStatus == VEHICLE_STATUS_WARNING  ? "WARNING"  :
                packet.vehicleStatus == VEHICLE_STATUS_CRITICAL ? "CRITICAL" : "INVALID",
                packet.temperature, packet.gasLevel, packet.voltage);

            // Post JSON to BLE notify queue
            if (xQueueSend(s_bleNotifyQueue, jsonBuf, 0) != pdPASS) {
                // Overwrite old item
                char discard[JSON_BUF_SIZE];
                xQueueReceive(s_bleNotifyQueue, discard, 0);
                xQueueSend(s_bleNotifyQueue, jsonBuf, 0);
            }
        } else {
            Serial.println("[TELEMETRY] WARNING: No sensor data received within timeout");
        }
    }
}

// ─── BLE Notify Task (Core 0) ─────────────────────────────────────────────────

static void ble_notify_task(void* pvParameters) {
    esp_task_wdt_add(NULL);

    static char jsonBuf[JSON_BUF_SIZE];
    TelemetryPacket_t dummy; // For structured notify call

    while (true) {
        esp_task_wdt_reset();

        if (xQueueReceive(s_bleNotifyQueue, jsonBuf, pdMS_TO_TICKS(2000)) == pdPASS) {
            if (ble_server_is_connected()) {
                // Pass dummy packet for per-characteristic individual values
                // The full JSON is used for VehicleStatus and FaultAlerts characteristics
                // Note: In a production implementation, deserialize back from JSON or
                // pass the packet through a second queue. For this prototype, the JSON
                // string carries all data the app needs via VehicleStatus characteristic.
                memset(&dummy, 0, sizeof(dummy));
                ble_server_notify(&dummy, jsonBuf);
            }
        }
    }
}

// ─── Setup & Loop ─────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(500); // Allow serial to stabilize

    diagnostics_boot_banner();

    // Enable watchdog
    esp_task_wdt_init(WDT_TIMEOUT_S, true);

    // Initialize hardware
    bool sensors_ok = sensors_init();
    diagnostics_init_result("Sensors", sensors_ok);

    ble_server_init();
    diagnostics_init_result("BLE", true);

    diagnostics_log_heap();

    // Create inter-task queues
    s_sensorQueue    = xQueueCreate(1, sizeof(SensorReadings_t));
    s_bleNotifyQueue = xQueueCreate(2, JSON_BUF_SIZE);

    if (!s_sensorQueue || !s_bleNotifyQueue) {
        Serial.println("[INIT] FATAL: Queue creation failed — halting");
        while (true) { delay(1000); }
    }

    // Create tasks
    xTaskCreatePinnedToCore(sensor_task,      "SensorTask",
                            TASK_STACK_SENSOR,    nullptr,
                            TASK_PRIORITY_SENSOR, nullptr, 1);

    xTaskCreatePinnedToCore(telemetry_task,   "TelemetryTask",
                            TASK_STACK_TELEMETRY, nullptr,
                            TASK_PRIORITY_TELEMETRY, nullptr, 1);

    xTaskCreatePinnedToCore(ble_notify_task,  "BleNotifyTask",
                            TASK_STACK_BLE,       nullptr,
                            TASK_PRIORITY_BLE,    nullptr, 0);

    Serial.println("[INIT] All tasks started. SafeCell AI running.");
}

void loop() {
    // All work is done in FreeRTOS tasks.
    // The Arduino loop task is deleted to free up its stack.
    esp_task_wdt_add(NULL);
    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(10000));
}
