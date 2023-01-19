#ifndef TRANSMITWIFI_H
#define TRANSMITWIFI_H

#include "defs.h"
#include "functions.h"

void mqttCallback(char *topic, byte *message, unsigned int length);

void setup_wifi();
void create_Accesspoint();

void reconnect();

void sendTelemetryWiFi(Data sv);

void handleWiFi(Data sv);

#endif