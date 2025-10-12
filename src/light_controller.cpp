#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>
#include <PubSubClient.h>

#include "light_controller.h"
#include "config.h"
#include "connections.h" // For the global 'client' object

// --- Private Objects and Variables for this Module ---
namespace {
  Adafruit_VEML7700 veml;

  // --- State Tracking Variables ---
  bool lightIsOn = false;
  bool lightManualOverride = false;
  unsigned long lastMotionTime = 0;
  unsigned long lightOnTime = 0;
  int pirState = LOW;
  int lastPirState = LOW;

  // --- Timer Durations ---
  unsigned long motionTimerDuration = INITIAL_MOTION_TIMER_DURATION_MS;
  unsigned long manualTimerDuration = INITIAL_MANUAL_TIMER_DURATION_MS;

  // --- Non-Blocking Timers ---
  unsigned long lastLuxReadTime = 0;
  unsigned long lastTimerRemainingPublish = 0;
  const int LUX_READ_INTERVAL = 1000; // Read lux every 1 seconds
  const int TIMER_REMAINING_PUBLISH_INTERVAL = 1000; // Publish countdown every 1 second
}

// --- Setup Function ---
void setup_light_controller() {
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LIGHT_RELAY_PIN, LOW);

  // Initialize Lux Sensor
  Serial.println("Initializing VEML7700 Lux Sensor...");
  if (!veml.begin()) {
    Serial.println("Failed to find VEML7700 chip");
  } else {
    Serial.println("VEML7700 Initialized.");
    veml.setGain(VEML7700_GAIN_1);
    veml.setIntegrationTime(VEML7700_IT_100MS);
  }
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

  // --- Core Light Logic ---
  if (!lightManualOverride && pirState == HIGH) {
    lastMotionTime = millis(); // Re-trigger timer on new motion
  }

  unsigned long currentTimerDuration = lightManualOverride ? manualTimerDuration : motionTimerDuration;
  bool timerIsActive = (millis() - lastMotionTime < currentTimerDuration);

  // --- Occupancy and Relay Control ---
  if (timerIsActive && !lightIsOn) {
    // Turn the light ON
    lightIsOn = true;
    digitalWrite(LIGHT_RELAY_PIN, HIGH);
    lightOnTime = millis();
    client.publish(MQTT_TOPIC_LIGHT_STATE, MQTT_PAYLOAD_ON, true);
    client.publish(MQTT_TOPIC_OCCUPANCY_STATE, MQTT_PAYLOAD_ON, true);
    Serial.println("Light and Occupancy turned ON.");

  } else if (!timerIsActive && lightIsOn) {
    // Turn the light OFF
    lightIsOn = false;
    digitalWrite(LIGHT_RELAY_PIN, LOW);
    client.publish(MQTT_TOPIC_LIGHT_STATE, MQTT_PAYLOAD_OFF, true);
    client.publish(MQTT_TOPIC_OCCUPANCY_STATE, MQTT_PAYLOAD_OFF, true);
    Serial.println("Light and Occupancy turned OFF.");

    // If it was a manual override, return to auto mode
    if (lightManualOverride) {
      lightManualOverride = false;
      Serial.println("Manual override timer expired. Returning to auto mode.");
    }
  }

  // --- Publish Lux periodically ---
  if (millis() - lastLuxReadTime > LUX_READ_INTERVAL) {
    lastLuxReadTime = millis();
    float currentLux = veml.readLux();
    char payload[10];
    dtostrf(currentLux, 1, 2, payload);
    client.publish(MQTT_TOPIC_LUX_STATE, payload, true);
  }

  // --- Publish Timer Remaining periodically ---
  if (millis() - lastTimerRemainingPublish > TIMER_REMAINING_PUBLISH_INTERVAL) {
    lastTimerRemainingPublish = millis();
    if (lightIsOn) {
      long timeRemainingMs = (lastMotionTime + currentTimerDuration) - millis();
      if (timeRemainingMs < 0) timeRemainingMs = 0;
      
      char payload[10];
      sprintf(payload, "%ld", timeRemainingMs / 1000); // Convert to seconds
      client.publish(MQTT_TOPIC_TIMER_REMAINING_STATE, payload, true);
    } else {
      // Publish 0 if the light is off
      client.publish(MQTT_TOPIC_TIMER_REMAINING_STATE, "0", true);
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
  }
}

void handle_motion_timer_command(String message) {
  unsigned long newDurationSec = message.toInt();
  if (newDurationSec >= 10 && newDurationSec <= 3600) {
    motionTimerDuration = newDurationSec * 1000;
    client.publish(MQTT_TOPIC_MOTION_TIMER_STATE, message.c_str(), true);
    Serial.print("Motion timer updated to ");
    Serial.println(message);
  }
}

void handle_manual_timer_command(String message) {
  unsigned long newDurationSec = message.toInt();
  if (newDurationSec >= 10 && newDurationSec <= 3600) {
    manualTimerDuration = newDurationSec * 1000;
    client.publish(MQTT_TOPIC_MANUAL_TIMER_STATE, message.c_str(), true);
    Serial.print("Manual timer updated to ");
    Serial.println(message);
  }
}

// --- Data Getters ---
unsigned long get_motion_timer_duration() {
  return motionTimerDuration;
}

unsigned long get_manual_timer_duration() {
  return manualTimerDuration;
}
