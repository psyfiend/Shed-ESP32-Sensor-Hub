#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Include our modularized files
#include "ota_manager.h"
#include "config.h"
#include "connections.h"
#include "light_controller.h"
#include "sensors.h"

// --- Global Objects ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- Non-Blocking Timers ---
unsigned long lastMqttReconnectAttempt = 0;

void setup() {
  Serial.begin(115200);

  setup_light_controller(); // Set up the pins and sensors for the light controller
  setup_environmental_sensors(); // Set up environmental sensors

  setup_wifi();
  setup_ota();
  
  // Configure MQTT client
  client.setServer(MQTT_SERVER, 1883);
  client.setBufferSize(DEVICE_DISCOVERY_PAYLOAD_SIZE);
  client.setCallback(mqtt_callback);
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastMqttReconnectAttempt > 5000) {
      lastMqttReconnectAttempt = now;
      reconnect();
    }
  } else {
    client.loop();
  }

  loop_ota(); // Handle OTA updates
  loop_light_controller(); // Run the core logic for the light controller
  read_environmental_sensors(); // Read environmental sensors
}
