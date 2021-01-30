/*
*   ESP8266 Template
*   Hardware / Board specific Settings
*/
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#ifdef D1_MINI
#define LED D4
// LED is inverted on D1 Mini
#define LEDON LOW
#define LEDOFF HIGH
// Corrected milliVolt-Divider (might need tuning for your ESP), default is 1024
#define VCCCORRDIV 919
#endif

#endif //HARDWARE_CONFIG_H