/*
*   ESP8266 Template
*   Common Function declarations
*/
#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <Arduino.h>
#include "wifi-config.h"
#include "mqtt-ota-config.h"
#include "generic-config.h"
#include "hardware-config.h"

void ToggleLed(int PIN, int WaitTime, int Count);
void MqttCallback(char *topic, byte *payload, unsigned int length);

#endif //COMMON_FUNCTIONS_H