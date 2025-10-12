#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

static const int DEVICE_DISCOVERY_PAYLOAD_SIZE = 8192; // Size of the JSON payload for MQTT Discovery

// --- Device Configuration ---
extern const char* DEVICE_ID;

// --- Hardware Pin Definitions (ESP32-C6) ---
extern const int PIR_SENSOR_PIN;
extern const int LIGHT_RELAY_PIN;
extern const int LED_PIN; // On-board LED for PIR indication

// --- Wi-Fi Credentials ---
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// --- MQTT Broker Settings ---
extern const char* MQTT_SERVER;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// --- Application Logic ---
extern unsigned long INITIAL_MOTION_TIMER_DURATION_MS;
extern unsigned long INITIAL_MANUAL_TIMER_DURATION_MS;

// --- MQTT Topics ---

// --- Device Availability ---
extern const char* MQTT_TOPIC_DEVICE_AVAILABILITY;

// --- Light Control ---
extern const char* MQTT_BASE_TOPIC_LIGHT;
extern const char* MQTT_TOPIC_LIGHT_STATE;
extern const char* MQTT_TOPIC_LIGHT_COMMAND;

// --- Timers ---
extern const char* MQTT_BASE_TOPIC_MOTION_TIMER;
extern const char* MQTT_TOPIC_MOTION_TIMER_STATE;
extern const char* MQTT_TOPIC_MOTION_TIMER_COMMAND;

extern const char* MQTT_BASE_TOPIC_MANUAL_TIMER;
extern const char* MQTT_TOPIC_MANUAL_TIMER_STATE;
extern const char* MQTT_TOPIC_MANUAL_TIMER_COMMAND;

// --- Sensors ---
extern const char* MQTT_TOPIC_MOTION_STATE;
extern const char* MQTT_TOPIC_OCCUPANCY_STATE; // NEW
extern const char* MQTT_TOPIC_LUX_STATE;
extern const char* MQTT_TOPIC_TIMER_REMAINING_STATE; // NEW

// --- MQTT Payloads ---
extern const char* MQTT_PAYLOAD_ONLINE;
extern const char* MQTT_PAYLOAD_OFFLINE;
extern const char* MQTT_PAYLOAD_ON;
extern const char* MQTT_PAYLOAD_OFF;

#endif // CONFIG_H

