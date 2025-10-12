#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include <String.h>

// --- Public Interface for the Light Controller Module ---

// Call this from setup()
void setup_light_controller();

// Call this from loop()
void loop_light_controller();

// --- MQTT Command Handlers ---
// These will be called by mqtt_callback in connections.cpp
void handle_light_command(String message);
void handle_motion_timer_command(String message);
void handle_manual_timer_command(String message);

// --- Data Getters ---
// For publishing initial state on MQTT reconnect
// unsigned long get_motion_timer_duration();
// unsigned long get_manual_timer_duration();
unsigned long get_current_timer_duration();

#endif // LIGHT_CONTROLLER_H
