#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <PubSubClient.h>

// "extern" tells the compiler that this object exists, but is defined
// in a different file (in our case, main.cpp).
extern PubSubClient client;

// This is the public list of functions available from this module.
void setup_wifi();
void reconnect();
void mqtt_callback(char* topic, byte* payload, unsigned int length);

#endif // CONNECTIONS_H
