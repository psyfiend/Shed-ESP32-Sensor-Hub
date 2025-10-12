#include "config.h"
#include <Arduino.h> // For LED_BUILTIN



// --- Device Configuration ---
const char* DEVICE_ID = "shed_sensor_hub";

// --- Hardware Pin Definitions (ESP32-C6) ---
const int PIR_SENSOR_PIN = 16;  // D6 is GPIO16
const int LIGHT_RELAY_PIN = 17; // D7 is GPIO17
const int LED_PIN = LED_BUILTIN; // On-board LED for PIR indication

// --- Wi-Fi Credentials ---
const char* WIFI_SSID = "M&M Motors";
const char* WIFI_PASSWORD = "seamosss";

// --- MQTT Broker Settings ---
const char* MQTT_SERVER = "192.168.0.70";
const char* MQTT_USER = "mqtt_user";
const char* MQTT_PASSWORD = "mqtt_user3700";

// --- Application Logic ---
unsigned long INITIAL_MOTION_TIMER_DURATION_MS = 60000;  // 60 seconds
unsigned long INITIAL_MANUAL_TIMER_DURATION_MS = 300000; // 5 minutes

// --- MQTT Topics ---

// --- Device Availability ---
const char* MQTT_TOPIC_DEVICE_AVAILABILITY = "devices/shed_sensor_hub/status";

// --- Light Control ---
const char* MQTT_BASE_TOPIC_LIGHT = "home/shed/light/main";
const char* MQTT_TOPIC_LIGHT_STATE = "home/shed/light/main/state";
const char* MQTT_TOPIC_LIGHT_COMMAND = "home/shed/light/main/command";

// --- Timers ---
const char* MQTT_BASE_TOPIC_MOTION_TIMER = "home/shed/number/motion_timer";
const char* MQTT_TOPIC_MOTION_TIMER_STATE = "home/shed/number/motion_timer/state";
const char* MQTT_TOPIC_MOTION_TIMER_COMMAND = "home/shed/number/motion_timer/command";

const char* MQTT_BASE_TOPIC_MANUAL_TIMER = "home/shed/number/manual_timer";
const char* MQTT_TOPIC_MANUAL_TIMER_STATE = "home/shed/number/manual_timer/state";
const char* MQTT_TOPIC_MANUAL_TIMER_COMMAND = "home/shed/number/manual_timer/command";

// --- Sensors ---
const char* MQTT_TOPIC_MOTION_STATE = "home/shed/binary_sensor/motion/state";
const char* MQTT_TOPIC_OCCUPANCY_STATE = "home/shed/binary_sensor/occupancy/state"; // NEW
const char* MQTT_TOPIC_LUX_STATE = "home/shed/sensor/lux/state";
const char* MQTT_TOPIC_TIMER_REMAINING_STATE = "home/shed/sensor/light_timer_remaining/state"; // NEW

// --- MQTT Payloads ---
const char* MQTT_PAYLOAD_ONLINE = "online";
const char* MQTT_PAYLOAD_OFFLINE = "offline";
const char* MQTT_PAYLOAD_ON = "ON";
const char* MQTT_PAYLOAD_OFF = "OFF";

