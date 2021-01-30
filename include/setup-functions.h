/*
*   ESP32 Template
*   Setup Function declarations
*/
#ifndef SETUP_FUNCTIONS_H
#define SETUP_FUNCTIONS_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include "wifi-config.h"
#include "generic-config.h"
#include "mqtt-ota-config.h"
#include "common-functions.h"

void wifi_setup();
void ota_setup();
void hardware_setup();

#endif //SETUP_FUNCTIONS_H