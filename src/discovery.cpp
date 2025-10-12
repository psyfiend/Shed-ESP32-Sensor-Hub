#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "discovery.h"
#include "config.h"

// This function needs access to the global MQTT client object
extern PubSubClient client;

void mqtt_discovery() {

    // Build and publish discovery json for the device and all components
    JsonDocument discovery_doc;
    const char* discovery_topic = "homeassistant/device/shed_sensor_hub/config"; // Unique topic for this device

    // Device document
    JsonObject device_doc = discovery_doc["device"].to<JsonObject>();
    device_doc["name"] = "Shed Sensor Hub";
    device_doc["ids"] = DEVICE_ID;  // shed_solar_power_monitor
    device_doc["mf"] = "Psyki Heavy Industries - Gem Systems";
    device_doc["mdl"] = "ESP32-C6 Sensor Core";
    device_doc["suggested_area"] = "Shed";

    // Origin document
    JsonObject origin_doc = discovery_doc["o"].to<JsonObject>();
    origin_doc["name"] = "Shed Sensor Control System";
    origin_doc["sw"] = "0.1";
    origin_doc["url"] = "https://switz.org";

    JsonObject cmps_doc = discovery_doc["cmps"].to<JsonObject>();

    // Light
    JsonObject light_cmp = cmps_doc["shed_main_light"].to<JsonObject>();
    light_cmp["name"] = "Shed Main Light";
    light_cmp["p"] = "light";
    light_cmp["uniq_id"] = "shed_sensor_hub_main_light";
    light_cmp["object_id"] = "shed_main_light";
    light_cmp["~"] = MQTT_BASE_TOPIC_LIGHT;                 // home/shed/light/main
    light_cmp["stat_t"] = "~/state";                        // home/shed/light/main/state
    light_cmp["cmd_t"] = "~/command";                       // home/shed/light/main/command
    light_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;   // devices/shed_sensor_hub/status
    light_cmp["pl_on"] = MQTT_PAYLOAD_ON;
    light_cmp["pl_off"] = MQTT_PAYLOAD_OFF;

    // Motion activated light timer
    JsonObject motion_timer_cmp = cmps_doc["shed_motion_timer"].to<JsonObject>();
    motion_timer_cmp["name"] = "Shed Motion Timer";
    motion_timer_cmp["p"] = "number";
    motion_timer_cmp["min"] = 10;
    motion_timer_cmp["max"] = 3600;
    motion_timer_cmp["unit_of_meas"] = "s";
    motion_timer_cmp["uniq_id"] = "shed_sensor_hub_motion_timer";
    motion_timer_cmp["object_id"] = "shed_motion_timer";
    motion_timer_cmp["~"] = MQTT_BASE_TOPIC_MOTION_TIMER; 		    // home/shed/number/motion_timer
    motion_timer_cmp["stat_t"] = "~/state";                         // home/shed/number/motion_timer/state
    motion_timer_cmp["cmd_t"] = "~/command";                        // home/shed/number/motion_timer/command
    motion_timer_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;    // devices/shed_sensor_hub/status

    // Manual override light timer
    JsonObject manual_timer_cmp = cmps_doc["shed_manual_timer"].to<JsonObject>();
    manual_timer_cmp["name"] = "Shed Manual Timer";
    manual_timer_cmp["p"] = "number";
    manual_timer_cmp["min"] = 10;
    manual_timer_cmp["max"] = 3600;
    manual_timer_cmp["unit_of_meas"] = "s";
    manual_timer_cmp["uniq_id"] = "shed_sensor_hub_manual_timer";
    manual_timer_cmp["object_id"] = "shed_manual_timer";
    manual_timer_cmp["~"] = MQTT_BASE_TOPIC_MANUAL_TIMER; 		    // home/shed/number/manual_timer
    manual_timer_cmp["stat_t"] = "~/state";                         // home/shed/number/manual_timer/state
    manual_timer_cmp["cmd_t"] = "~/command";                        // home/shed/number/manual_timer/command
    manual_timer_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;    // devices/shed_sensor_hub/status
    
    // Sensors
    // Motion sensor
    JsonObject motion_sensor_cmp = cmps_doc["shed_motion_sensor"].to<JsonObject>();
    motion_sensor_cmp["name"] = "Shed Motion";
    motion_sensor_cmp["p"] = "binary_sensor";
    motion_sensor_cmp["dev_cla"] = "motion";
    motion_sensor_cmp["stat_on"] = "ON";
    motion_sensor_cmp["stat_off"] = "OFF";
    motion_sensor_cmp["uniq_id"] = "shed_sensor_hub_motion_sensor";
    motion_sensor_cmp["object_id"] = "shed_motion_sensor";
    motion_sensor_cmp["stat_t"] = MQTT_TOPIC_MOTION_STATE;           // home/shed/binary_sensor/motion/state
    motion_sensor_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;   // devices/shed_sensor_hub/status
    motion_sensor_cmp["pl_on"] = MQTT_PAYLOAD_ON;
    motion_sensor_cmp["pl_off"] = MQTT_PAYLOAD_OFF;

    // Occupancy sensor
    JsonObject occupancy_sensor_cmp = cmps_doc["shed_occupancy_sensor"].to<JsonObject>();
    occupancy_sensor_cmp["name"] = "Shed Occupancy";
    occupancy_sensor_cmp["p"] = "binary_sensor";
    occupancy_sensor_cmp["dev_cla"] = "occupancy";
    occupancy_sensor_cmp["stat_on"] = "ON";
    occupancy_sensor_cmp["stat_off"] = "OFF";
    occupancy_sensor_cmp["uniq_id"] = "shed_sensor_hub_occupancy_sensor";
    occupancy_sensor_cmp["object_id"] = "shed_occupancy_sensor";
    occupancy_sensor_cmp["stat_t"] = MQTT_TOPIC_OCCUPANCY_STATE;        // home/shed/binary_sensor/occupancy/state
    occupancy_sensor_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;    // devices/shed_sensor_hub/status
    occupancy_sensor_cmp["pl_on"] = MQTT_PAYLOAD_ON;
    occupancy_sensor_cmp["pl_off"] = MQTT_PAYLOAD_OFF;

    // Ambient Light Sensor (lux)
    JsonObject lux_sensor_cmp = cmps_doc["shed_lux_sensor"].to<JsonObject>();
    lux_sensor_cmp["name"] = "Shed Ambient Light";
    lux_sensor_cmp["p"] = "sensor";
    lux_sensor_cmp["dev_cla"] = "illuminance";
    lux_sensor_cmp["unit_of_meas"] = "lx";
    lux_sensor_cmp["stat_cla"] = "measurement";
    lux_sensor_cmp["uniq_id"] = "shed_sensor_hub_lux_sensor";
    lux_sensor_cmp["object_id"] = "shed_lux_sensor";
    lux_sensor_cmp["stat_t"] = MQTT_TOPIC_LUX_STATE;            // home/shed/sensor/lux/state
    lux_sensor_cmp["avty_t"] = MQTT_TOPIC_DEVICE_AVAILABILITY;  // devices/shed_sensor_hub/status
    lux_sensor_cmp["val_tpl"] = "{{ value | float }}";          // Ensure the value is treated as a float

    // Print the JSON document to Serial for debugging
    Serial.println("--- MQTT Discovery Payload ---");
    serializeJsonPretty(discovery_doc, Serial);
    Serial.println();
    Serial.print("Discovery Topic: ");
    Serial.println(discovery_topic);

    // Print the total size of the JSON document
    size_t jsonSize = measureJson(discovery_doc);
    Serial.println("--------------------------------");
    Serial.print("Total JSON size: ");
    Serial.println(jsonSize);
    Serial.println("--------------------------------");

    if (jsonSize < DEVICE_DISCOVERY_PAYLOAD_SIZE) {
        Serial.println("Publishing discovery document to MQTT broker...");
        static char buffer[DEVICE_DISCOVERY_PAYLOAD_SIZE];
        serializeJson(discovery_doc, buffer);
        client.publish(discovery_topic, buffer, true);
    } else {
        Serial.println("Error: JSON document size exceeds buffer size.");
    }
}