#include <WiFi.h>
#include <PubSubClient.h>
#include "connections.h"
#include "config.h"
#include "discovery.h"      // For MQTT discovery message
#include "light_controller.h" // To handle light commands and timer updates

// This requires the global client object defined in main.cpp
extern PubSubClient client;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.setHostname(DEVICE_ID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// --- MQTT Message Callback ---
// This function is the central router for all incoming MQTT messages.
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Convert the payload to a printable string
  payload[length] = '\0'; // Add a null terminator
  String message = (char*)payload;

  Serial.println("--- MQTT Message Received ---");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(message);
  Serial.println("-----------------------------");

  // ---- Route messages to the light controller based on topic ----
  if (String(topic) == MQTT_TOPIC_LIGHT_COMMAND) {
    handle_light_command(message);
  } else if (String(topic) == MQTT_TOPIC_MOTION_TIMER_COMMAND) {
    handle_motion_timer_command(message);
  } else if (String(topic) == MQTT_TOPIC_MANUAL_TIMER_COMMAND) {
    handle_manual_timer_command(message);
  }
}

// --- MQTT Reconnect Logic ---
void reconnect() {
  Serial.print("Attempting MQTT connection...");
  
  if (client.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_DEVICE_AVAILABILITY, 1, true, MQTT_PAYLOAD_OFFLINE)) {
    Serial.println("connected!");
    
    // Publish device availability
    client.publish(MQTT_TOPIC_DEVICE_AVAILABILITY, MQTT_PAYLOAD_ONLINE, true);
    
    // Publish the initial timer states (in seconds)
    String motion_payload = String(get_motion_timer_duration() / 1000);
    client.publish(MQTT_TOPIC_MOTION_TIMER_STATE, motion_payload.c_str(), true);

    String manual_payload = String(get_manual_timer_duration() / 1000);
    client.publish(MQTT_TOPIC_MANUAL_TIMER_STATE, manual_payload.c_str(), true);

    Serial.println("Published initial timer states.");
    
    // --- Subscribe to Command Topics ---
    Serial.println("------------------------------");
    client.subscribe(MQTT_TOPIC_LIGHT_COMMAND);
    client.subscribe(MQTT_TOPIC_MOTION_TIMER_COMMAND);
    client.subscribe(MQTT_TOPIC_MANUAL_TIMER_COMMAND);
    Serial.println("Subscribed to command topics.");

    // Publish the discovery message
    mqtt_discovery();

  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
  }
}
