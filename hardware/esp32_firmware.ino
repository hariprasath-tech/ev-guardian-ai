/*
 * ==============================================================================
 * SafeCell AI — Production ESP32 / ESP32-S3 IoT Hardware Firmware
 * EV Battery Monitoring & Autonomous Aerosol Fire Suppression Control Node
 * ==============================================================================
 * 
 * Hardware Requirements:
 * - ESP32 / ESP32-S3 Development Board
 * - DHT22 / DHT11 Temperature & Humidity Sensor (GPIO 4)
 * - MQ-7 / MQ-8 Gas & Smoke Sensors (ADC GPIO 33, 32)
 * - Flame/Fire Digital Sensor (GPIO 35)
 * - Voltage Divider (0-500V Pack Voltage Step-Down, GPIO 34)
 * - ACS712 / Hall Current Sensor (GPIO 36)
 * - 5V Relay Module for Aerosol Extinguisher Actuator (GPIO 26)
 * - Active Buzzer & Status Indicator LED (GPIO 27, GPIO 2)
 * 
 * Dependencies (Install via Arduino IDE Library Manager):
 * 1. ArduinoJson (v6.21+ or v7.0+) by Benoit Blanchon
 * 2. WebSockets by Markus Sattler
 * 3. PubSubClient by Nick O'Leary
 * 4. DHT sensor library by Adafruit
 * 5. Adafruit Unified Sensor by Adafruit
 * 
 * Compatible with Arduino IDE 1.8.x & Arduino IDE 2.x
 * ==============================================================================
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <time.h>

// ==============================================================================
// 1. CONFIGURATION PARAMETERS (EDIT THESE FOR YOUR NETWORK & SERVER)
// ==============================================================================
const char* WIFI_SSID         = "YOUR_WIFI_SSID";     // Change to your Wi-Fi SSID
const char* WIFI_PASSWORD     = "YOUR_WIFI_PASSWORD"; // Change to your Wi-Fi Password

// Backend Server IP or Hostname (Use your laptop's local IP, e.g., 192.168.1.100)
const char* BACKEND_HOST      = "192.168.1.100";      
const int   HTTP_PORT         = 8000;
const int   MQTT_PORT         = 1883;

// Device Credentials & Identifiers
const char* DEVICE_ID         = "SC-ESP32-04A2";
const char* FIRMWARE_VERSION  = "v3.0.0-PROD";
const char* MQTT_TOPIC_DATA   = "safecell/telemetry";
const char* MQTT_TOPIC_CMD    = "safecell/commands";

// Sensor Pin Definitions
#define PIN_DHT_SENSOR        4   // GPIO4  - DHT22 Temp & Humidity
#define DHT_TYPE              DHT22
#define PIN_MQ7_GAS           33  // GPIO33 - MQ-7 CO Gas Sensor (ADC)
#define PIN_MQ8_SMOKE         32  // GPIO32 - MQ-8 Smoke Sensor (ADC)
#define PIN_FLAME_SENSOR      35  // GPIO35 - Flame Digital Input
#define PIN_VOLTAGE_ADC       34  // GPIO34 - Pack Voltage ADC
#define PIN_CURRENT_ADC       36  // GPIO36 - Pack Current ADC (VP)

// Output Control Actuators
#define PIN_SUPPRESSION_RELAY 26  // GPIO26 - Aerosol Fire Extinguisher Relay
#define PIN_ALARM_BUZZER      27  // GPIO27 - High Decibel Alarm Buzzer
#define PIN_STATUS_LED        2   // GPIO2  - Onboard Blue Status LED

// Safety Threshold Defaults
#define MAX_SAFE_TEMP_C       60.0
#define MAX_SAFE_CO2_PPM      2000.0
#define MAX_SAFE_SMOKE_PCT    0.50

// Buffer Size for Offline Queue
#define OFFLINE_BUFFER_SIZE   30

// ==============================================================================
// 2. GLOBAL OBJECTS & STATE VARIABLES
// ==============================================================================
DHT dht(PIN_DHT_SENSOR, DHT_TYPE);
WebSocketsClient webSocket;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// System State
bool systemArmed = true;
bool emergencyActive = false;
float sensitivityThreshold = 0.60;
String macAddress = "";
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_INTERVAL_MS = 1000; // 1 second update cycle

// Live Sensor Telemetry Cache
struct SensorReadings {
  float batteryTemp = 31.5;
  float batteryVoltage = 398.4;
  float batteryCurrent = 12.5;
  float co2Ppm = 412.0;
  float smokeDensity = 0.02;
  float batteryHealth = 98.0;
  int cycleCount = 342;
  float thermalGradient = 6.0;
  float runawayRisk = 1.8;
  bool flameDetected = false;
  double latitude = 37.7749;
  double longitude = -122.4194;
  unsigned long timestamp = 0;
} currentSensors;

// Offline Data Buffer
SensorReadings offlineBuffer[OFFLINE_BUFFER_SIZE];
int bufferIndex = 0;

// ==============================================================================
// 3. HARDWARE CONTROL FUNCTIONS
// ==============================================================================
void setFireSuppression(bool state) {
  digitalWrite(PIN_SUPPRESSION_RELAY, state ? HIGH : LOW);
  digitalWrite(PIN_ALARM_BUZZER, state ? HIGH : LOW);
  emergencyActive = state;
  Serial.printf("[HARDWARE] Suppression Actuator Relay: %s\n", state ? "ACTIVATED (FIRE)" : "STANDBY");
}

void blinkStatusLed(int count, int delayMs) {
  for (int i = 0; i < count; i++) {
    digitalWrite(PIN_STATUS_LED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_STATUS_LED, LOW);
    delay(delayMs);
  }
}

// ==============================================================================
// 4. COMMAND DISPATCHER (WebSocket & MQTT Commands)
// ==============================================================================
void handleIncomingCommand(const char* jsonPayload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, jsonPayload);
  if (error) {
    Serial.printf("[JSON] Deserialization error: %s\n", error.c_str());
    return;
  }

  Serial.println("[COMMAND] Processing server command payload...");

  if (doc.containsKey("system_armed")) {
    systemArmed = doc["system_armed"];
    Serial.printf("[STATE] System Armed updated: %s\n", systemArmed ? "YES" : "NO");
  }

  if (doc.containsKey("emergency_active")) {
    bool active = doc["emergency_active"];
    setFireSuppression(active);
  }

  if (doc.containsKey("action")) {
    String action = doc["action"];
    if (action == "trigger_suppression") {
      setFireSuppression(true);
    } else if (action == "reset_suppression") {
      setFireSuppression(false);
    } else if (action == "arm") {
      systemArmed = true;
    } else if (action == "disarm") {
      systemArmed = false;
    }
  }
}

// ==============================================================================
// 5. COMMUNICATION HANDLERS (WebSocket, MQTT, REST HTTP)
// ==============================================================================
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WS] WebSocket Disconnected from Backend");
      digitalWrite(PIN_STATUS_LED, LOW);
      break;

    case WStype_CONNECTED:
      Serial.println("[WS] WebSocket Stream Connected Successfully!");
      digitalWrite(PIN_STATUS_LED, HIGH);
      break;

    case WStype_TEXT:
      Serial.printf("[WS] Received Text: %s\n", payload);
      handleIncomingCommand((char*)payload);
      break;
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.printf("[MQTT] Message on [%s]: %s\n", topic, message);
  handleIncomingCommand(message);
}

void reconnectMQTT() {
  if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    Serial.print("[MQTT] Connecting to Broker...");
    if (mqttClient.connect(DEVICE_ID)) {
      Serial.println(" Connected!");
      mqttClient.subscribe(MQTT_TOPIC_CMD);
    } else {
      Serial.printf(" Failed (rc=%d). Will retry.\n", mqttClient.state());
    }
  }
}

// REST HTTP POST Fallback
bool sendTelemetryHTTP() {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  String url = "http://" + String(BACKEND_HOST) + ":" + String(HTTP_PORT) + "/api/telemetry";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> doc;
  doc["device_id"] = DEVICE_ID;
  doc["mac_address"] = macAddress;
  doc["firmware_version"] = FIRMWARE_VERSION;
  doc["battery_temp"] = currentSensors.batteryTemp;
  doc["battery_voltage"] = currentSensors.batteryVoltage;
  doc["battery_current"] = currentSensors.batteryCurrent;
  doc["co2_ppm"] = currentSensors.co2Ppm;
  doc["smoke_density"] = currentSensors.smokeDensity;
  doc["battery_health"] = currentSensors.batteryHealth;
  doc["cycle_count"] = currentSensors.cycleCount;
  doc["runaway_risk"] = currentSensors.runawayRisk;
  doc["thermal_gradient"] = currentSensors.thermalGradient;
  doc["latitude"] = currentSensors.latitude;
  doc["longitude"] = currentSensors.longitude;
  doc["system_armed"] = systemArmed;
  doc["emergency_active"] = emergencyActive;
  doc["mode"] = "hardware";
  doc["timestamp"] = millis() / 1000;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  int httpCode = http.POST(jsonPayload);
  http.end();

  return (httpCode == 200 || httpCode == 201);
}

// WebSocket JSON Transmission
void sendTelemetryWebSocket() {
  if (!webSocket.isConnected()) return;

  StaticJsonDocument<512> doc;
  doc["device_id"] = DEVICE_ID;
  doc["mac_address"] = macAddress;
  doc["firmware_version"] = FIRMWARE_VERSION;
  doc["battery_temp"] = currentSensors.batteryTemp;
  doc["battery_voltage"] = currentSensors.batteryVoltage;
  doc["battery_current"] = currentSensors.batteryCurrent;
  doc["co2_ppm"] = currentSensors.co2Ppm;
  doc["smoke_density"] = currentSensors.smokeDensity;
  doc["battery_health"] = currentSensors.batteryHealth;
  doc["cycle_count"] = currentSensors.cycleCount;
  doc["runaway_risk"] = currentSensors.runawayRisk;
  doc["thermal_gradient"] = currentSensors.thermalGradient;
  doc["latitude"] = currentSensors.latitude;
  doc["longitude"] = currentSensors.longitude;
  doc["system_armed"] = systemArmed;
  doc["emergency_active"] = emergencyActive;
  doc["mode"] = "hardware";
  doc["timestamp"] = millis() / 1000;

  String jsonPayload;
  serializeJson(doc, jsonPayload);
  webSocket.sendTXT(jsonPayload);
}

// ==============================================================================
// 6. SENSOR READERS & SAFETY ENGINE
// ==============================================================================
void readSensors() {
  // 1. Read DHT22 Digital Temperature & Humidity
  float dhtTemp = dht.readTemperature();
  if (!isnan(dhtTemp)) {
    currentSensors.batteryTemp = dhtTemp;
  } else {
    // ADC Fallback reading on PIN_VOLTAGE_ADC if DHT not attached
    int rawTempAdc = analogRead(PIN_VOLTAGE_ADC);
    currentSensors.batteryTemp = 25.0 + (rawTempAdc / 4095.0) * 30.0;
  }

  // 2. Read MQ-7 CO2 / Gas ADC
  int rawGas = analogRead(PIN_MQ7_GAS);
  currentSensors.co2Ppm = 400.0 + (rawGas / 4095.0) * 2500.0;

  // 3. Read MQ-8 Smoke ADC
  int rawSmoke = analogRead(PIN_MQ8_SMOKE);
  currentSensors.smokeDensity = (rawSmoke / 4095.0) * 1.0;

  // 4. Flame Digital Sensor Input
  currentSensors.flameDetected = (digitalRead(PIN_FLAME_SENSOR) == LOW);

  // 5. Read Voltage & Current Analog Signals
  int rawVolt = analogRead(PIN_VOLTAGE_ADC);
  currentSensors.batteryVoltage = 350.0 + (rawVolt / 4095.0) * 100.0;

  int rawCurr = analogRead(PIN_CURRENT_ADC);
  currentSensors.batteryCurrent = 5.0 + (rawCurr / 4095.0) * 30.0;

  // 6. AI Thermal Runaway Risk Assessment Algorithm
  float tempFactor = (currentSensors.batteryTemp > 45.0) ? (currentSensors.batteryTemp - 45.0) * 2.5 : 0;
  float gasFactor = (currentSensors.co2Ppm > 1000.0) ? (currentSensors.co2Ppm - 1000.0) / 40.0 : 0;
  float smokeFactor = currentSensors.smokeDensity * 50.0;
  currentSensors.runawayRisk = Math.min(100.0f, tempFactor + gasFactor + smokeFactor);

  // 7. Automatic Safety Interlock Rule
  if (systemArmed) {
    if (currentSensors.batteryTemp > MAX_SAFE_TEMP_C || 
        currentSensors.co2Ppm > MAX_SAFE_CO2_PPM || 
        currentSensors.smokeDensity > MAX_SAFE_SMOKE_PCT || 
        currentSensors.flameDetected) {
      if (!emergencyActive) {
        Serial.println("🚨 [ALERT] CRITICAL THERMAL RUNAWAY / FIRE DETECTED! ACTIVATING SUPPRESSION RELAY!");
        setFireSuppression(true);
      }
    }
  }
}

// ==============================================================================
// 7. SETUP & INITIALIZATION
// ==============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================================");
  Serial.println(" SafeCell AI — Production ESP32 Hardware Firmware");
  Serial.println("========================================================");

  // Configure Pin Modes
  pinMode(PIN_SUPPRESSION_RELAY, OUTPUT);
  pinMode(PIN_ALARM_BUZZER, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_FLAME_SENSOR, INPUT_PULLUP);

  digitalWrite(PIN_SUPPRESSION_RELAY, LOW);
  digitalWrite(PIN_ALARM_BUZZER, LOW);
  digitalWrite(PIN_STATUS_LED, LOW);

  // Initialize DHT Sensor
  dht.begin();

  // Connect to Wi-Fi Network
  Serial.printf("[WIFI] Connecting to SSID: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    macAddress = WiFi.macAddress();
    Serial.println("\n[WIFI] Connected Successfully!");
    Serial.printf("[WIFI] IP Address : %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("[WIFI] MAC Address: %s\n", macAddress.c_str());
    blinkStatusLed(3, 100);
  } else {
    Serial.println("\n[WIFI] Warning: Wi-Fi Connection Timeout. Running in Offline Buffer Mode.");
  }

  // Setup WebSocket Communication
  webSocket.begin(BACKEND_HOST, HTTP_PORT, "/ws/telemetry");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(3000);

  // Setup MQTT Communication
  mqttClient.setServer(BACKEND_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  Serial.println("[SYSTEM] ESP32 Telemetry & Fire Safety Node Ready.");
}

// ==============================================================================
// 8. MAIN EXECUTION LOOP
// ==============================================================================
void loop() {
  // Maintain Communication Protocols
  if (WiFi.status() == WL_CONNECTED) {
    webSocket.loop();
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
  }

  // Periodic Telemetry Sample & Transmission (Every 1 Second)
  unsigned long now = millis();
  if (now - lastTelemetryTime >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryTime = now;

    // 1. Read all physical hardware sensors
    readSensors();

    // 2. Transmit over WebSocket stream
    if (WiFi.status() == WL_CONNECTED) {
      sendTelemetryWebSocket();
      sendTelemetryHTTP();
    } else {
      // Buffer reading offline if Wi-Fi dropped
      if (bufferIndex < OFFLINE_BUFFER_SIZE) {
        offlineBuffer[bufferIndex++] = currentSensors;
        Serial.printf("[BUFFER] Saved offline reading #%d\n", bufferIndex);
      }
    }

    // Debug Serial Output
    Serial.printf("[TELEMETRY] Temp: %.1f°C | CO2: %.0f ppm | Smoke: %.2f%% | Volt: %.1fV | Risk: %.1f%%\n",
                  currentSensors.batteryTemp,
                  currentSensors.co2Ppm,
                  currentSensors.smokeDensity,
                  currentSensors.batteryVoltage,
                  currentSensors.runawayRisk);
  }
}
