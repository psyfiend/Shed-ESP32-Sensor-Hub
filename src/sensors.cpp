#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_VEML7700.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "config.h"

extern PubSubClient client;

// --- Sensor Objects ---
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp; // I2C
Adafruit_VEML7700 veml;

// --- Non-Blocking Sensor Timers ---
unsigned long lastAHTReadTime = 0;
unsigned long lastBMPReadTime = 0;
unsigned long lastLUXReadTime = 0;

const int ahtReadInterval = 1000; // Read AHT10 every 1 second
const int bmpReadInterval = 1000; // Read BMP280 every 1 second
const int luxReadInterval = 1000; // Read LUX every 1 second

// Call this from setup()
void setup_environmental_sensors() {
    Serial.println("Initializing Environmental Sensors...");

    // AHT10 Temperature and Humidity Sensor Setup
    Serial.println("Initializing AHT10 Sensor...");
    if (!aht.begin()) {
        Serial.println("Failed to find AHT10 chip");
    } else {
        Serial.println("AHT10 Initialized.");
    }

    // BMP280 Pressure Sensor Setup
    Serial.println("Initializing BMP280 Sensor...");
    if (!bmp.begin()) {
        Serial.println("Failed to find BMP280 chip");
    } else {
        Serial.println("BMP280 Initialized.");
    }

    // VEML7700 Light Sensor Setup
    Serial.println("Initializing VEML7700 Sensor...");
    if (!veml.begin()) {
        Serial.println("Failed to find VEML7700 chip");
    } else {
        Serial.println("VEML7700 Initialized.");
        veml.setGain(VEML7700_GAIN_1);
        veml.setIntegrationTime(VEML7700_IT_100MS);
    }
    Serial.println("Environmental Sensors Initialized.");
    delay(500); // Pause for serial monitor
}

// Call this from loop()
void read_environmental_sensors() {
  unsigned long currentTime = millis();

  // Read AHT10 Sensor
  if (currentTime - lastAHTReadTime >= ahtReadInterval) {
    lastAHTReadTime = currentTime;
    sensors_event_t humidityEvent, tempEvent;
    aht.getEvent(&humidityEvent, &tempEvent);
    float temperatureF = (tempEvent.temperature * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit
    float humidity = humidityEvent.relative_humidity;
    client.publish(MQTT_TOPIC_TEMPERATURE_SHED_STATE, String(temperatureF).c_str());
    client.publish(MQTT_TOPIC_HUMIDITY_SHED_STATE, String(humidity).c_str());
  }

  // Read BMP280 Sensor
  if (currentTime - lastBMPReadTime >= bmpReadInterval) {
    lastBMPReadTime = currentTime;
    float pressure_hPa = bmp.readPressure() / 100.0F; // Convert to hPa
    client.publish(MQTT_TOPIC_PRESSURE_SHED_STATE, String(pressure_hPa).c_str());
  }

  // Read VEML7700 Sensor
  if (currentTime - lastLUXReadTime >= luxReadInterval) {
    lastLUXReadTime = currentTime;
    float luxValue = veml.readLux();
    char payload[10];
    dtostrf(luxValue, 1, 2, payload);
    client.publish(MQTT_TOPIC_LUX_SHED_STATE, payload, true);
  }
}