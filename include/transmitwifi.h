#ifndef TRANSMITWIFI_H
#define TRANSMITWIFI_H

#include "defs.h"

void mqttCallback(char *topic, byte *message, unsigned int length);

void setup_wifi();

void reconnect();

void sendTelemetryWiFi(SendValues sv[5]);

void handleWiFi(SendValues sv[5]);

#endif
