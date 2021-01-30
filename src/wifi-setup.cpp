/*
 * ESP8266 Template
 * WiFi specific setup
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "wifi-config.h"

// Define Variables
const char *ssid = WIFI_SSID;
const char *password = WIFI_PSK;
