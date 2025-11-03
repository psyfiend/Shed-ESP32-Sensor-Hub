#include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_VEML7700.h>
#include <PubSubClient.h>
#include "light_controller.h"
#include "config.h"
#include "connections.h" // For the global 'client' object

extern PubSubClient client;

// --- State Tracking Variables ---
// Adafruit_VEML7700 veml;
bool lightIsOn = false;
bool lightManualOverride = false;
unsigned long lastMotionTime = 0;
unsigned long lightOnTime = 0;
int pirState = LOW;
int lastPirState = LOW;
unsigned long lastTimerRemainingPublishTime = 0;

// --- Timer Durations ---
unsigned long motionTimerDuration = INITIAL_MOTION_TIMER_DURATION_MS;
unsigned long manualTimerDuration = INITIAL_MANUAL_TIMER_DURATION_MS;

// --- Non-Blocking Timers ---
// unsigned long lastLuxReadTime = 0;
// unsigned long lastTimerRemainingPublish = 0;
// const int LUX_READ_INTERVAL = 1000; // Read lux every 1 seconds
// const int TIMER_REMAINING_PUBLISH_INTERVAL = 1000; // Publish countdown every 1 second

// --- Private Function Prototypes ---
unsigned long get_current_timer_duration();

// --- Setup Function ---
void setup_light_controller() {
  Serial.println("Initializing Light Controller...");
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LIGHT_RELAY_PIN, LOW);

  // Initialize Lux Sensor
  // Serial.println("Initializing VEML7700 Lux Sensor...");
  // if (!veml.begin()) {
    // Serial.println("Failed to find VEML7700 chip");
  // } else {
    // Serial.println("VEML7700 Initialized.");
    // veml.setGain(VEML7700_GAIN_1);
    // veml.setIntegrationTime(VEML7700_IT_100MS);
  // }
  Serial.println("Light Controller Initialized.");
  delay(500); // Pause for serial monitor
}

// --- Main Loop Function ---
void loop_light_controller() {
  // --- Read Sensors ---
  pirState = digitalRead(PIR_SENSOR_PIN);
  digitalWrite(LED_PIN, pirState); // Update onboard LED for visual feedback

  // --- Publish Raw PIR State Changes ---
  if (pirState != lastPirState) {
    client.publish(MQTT_TOPIC_MOTION_STATE, pirState == HIGH ? MQTT_PAYLOAD_ON : MQTT_PAYLOAD_OFF, true);
    lastPirState = pirState;
  }

  // --- Publish Lux periodically ---
  // if (millis() - lastLuxReadTime > LUX_READ_INTERVAL) {
    // lastLuxReadTime = millis();
    // float currentLux = veml.readLux();
    // char payload[10];
    // dtostrf(currentLux, 1, 2, payload);
    // client.publish(MQTT_TOPIC_LUX_SHED_STATE, payload, true);
  // }

  // --- Core Light Logic ---
  if (!lightManualOverride && pirState == HIGH) {
    lastMotionTime = millis(); // Re-trigger timer on new motion
  }

  unsigned long currentTimerDuration = get_current_timer_duration();
  bool relayShouldBeOn = (millis() - lastMotionTime < currentTimerDuration);
  unsigned long timeSinceLightOn = millis() - lightOnTime;
  unsigned long timerRemainingSeconds = (timeSinceLightOn > currentTimerDuration) ? 0 : (currentTimerDuration - timeSinceLightOn) / 1000;

  // --- Occupancy and Relay Control ---
  if (relayShouldBeOn && !lightIsOn) {
    // Turn the light ON
    lightIsOn = true;
    lightOnTime = millis();
    Serial.println(lightManualOverride ? "Manual override: Turning relay ON." : "Occupancy detected: Turning relay ON.");
    digitalWrite(LIGHT_RELAY_PIN, HIGH);
    if (!lightManualOverride) {
      client.publish(MQTT_TOPIC_OCCUPANCY_STATE, MQTT_PAYLOAD_ON, true);
    }
    client.publish(MQTT_TOPIC_LIGHT_STATE, MQTT_PAYLOAD_ON, true);

  } else if (!relayShouldBeOn && lightIsOn) {
    // Turn the light OFF
    lightIsOn = false;
    Serial.println("No occupancy: Turning relay OFF.");
    digitalWrite(LIGHT_RELAY_PIN, LOW);
    client.publish(MQTT_TOPIC_OCCUPANCY_STATE, MQTT_PAYLOAD_OFF, true);
    client.publish(MQTT_TOPIC_LIGHT_STATE, MQTT_PAYLOAD_OFF, true);

    // Publish a final "0" for timer remaining
    client.publish(MQTT_TOPIC_TIMER_REMAINING_STATE, "0", true);

    // If it was a manual override, return to auto mode
    if (lightManualOverride) {
      lightManualOverride = false;
      Serial.println("Manual override timer expired. Returning to auto mode.");
    }
  }

  // Publish the remaining time for UI but only if the light is on
  if (lightIsOn) {
    if (millis() - lastTimerRemainingPublishTime > 1000) { // Every second
      lastTimerRemainingPublishTime = millis();
      char payload[10];
      dtostrf(timerRemainingSeconds, 1, 0, payload);
      client.publish(MQTT_TOPIC_TIMER_REMAINING_STATE, payload, true);
    }
  }
}


// --- MQTT Command Handlers ---
void handle_light_command(String message) {
  message.toUpperCase();
  if (message == "ON") {
    lightManualOverride = true;
    lastMotionTime = millis(); // Start the manual timer
    Serial.println("Received command: Manual ON");
  } else if (message == "OFF") {
    lightManualOverride = false;
    // Expire the timer immediately to turn the light off in the next loop
    lastMotionTime = millis() - motionTimerDuration - 1;
    Serial.println("Received command: Manual OFF");
  } else if (message == "TOGGLE") {
    // Toggle the manual override state
    handle_light_command(lightIsOn ? "OFF" : "ON");
    Serial.println("Received command: TOGGLE");
  }
}

// --- Hardened MQTT Numerical Handlers ---
void handle_motion_timer_command(String message) {
  char* endPtr;
  unsigned long newDurationSec = strtoul(message.c_str(), &endPtr, 10);

  // Check if conversion was successful and within valid range
  if (*endPtr == '\0' && newDurationSec >= 10 && newDurationSec <= 3600) {
    motionTimerDuration = newDurationSec * 1000;
    Serial.print("Motion timer updated to ");
    Serial.print(newDurationSec);
    Serial.println(" seconds.");
    // Acknowledge the change by publishing the new state
    client.publish(MQTT_TOPIC_MOTION_TIMER_STATE, message.c_str(), true);
  } else {
    Serial.printf("Received invalid motion timer duration: %s. Must be between 10 and 3600 seconds.\n", message.c_str());
  }
}

void handle_manual_timer_command(String message) {
  char* endPtr;
  unsigned long newDurationSec = strtoul(message.c_str(), &endPtr, 10);

  // Check if conversion was successful and within valid range
  if (*endPtr == '\0' && newDurationSec >= 10 && newDurationSec <= 3600) {
    manualTimerDuration = newDurationSec * 1000;
    Serial.print("Manual timer updated to ");
    Serial.print(newDurationSec);
    Serial.println(" seconds.");
    // Acknowledge the change by publishing the new state
    client.publish(MQTT_TOPIC_MANUAL_TIMER_STATE, message.c_str(), true);
  } else {
    Serial.printf("Received invalid manual timer duration: %s. Must be between 10 and 3600 seconds.\n", message.c_str());
  }
}

// --- Private Helper Functions ---
unsigned long get_current_timer_duration() {
  return lightManualOverride ? manualTimerDuration : motionTimerDuration;
}
