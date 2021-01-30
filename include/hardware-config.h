/*
*   ESP32 Template
*   Hardware / Board specific Settings
*/
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include "driver/adc.h"

#ifdef WEMOS_LOLIN32
// Onboard LED
#define LED 5
// LED is inverted on board
#define LEDON LOW
#define LEDOFF HIGH

// Pin used for reading VCC (ADC1 channel 7, Pin 35)
// ==================================================
// ATTN: i have no idea how this supposed to work, as it seems that there is no
// reference voltage (other than VCC, which is useless of course) for the ADC
// with 6dB attenuation Full Scale voltage *should be* 2.2V... use 100k/100k voltage divider (factor 2) on board
// DOCS: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
// and: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// BOARD: https://www.wemos.cc/en/latest/_static/files/sch_d32_v1.0.0.pdf
#define VDIV 2.0f
#define VFULL_SCALE 2.2f
#define VBAT_ADC_PIN 35
#define ADC_CHAN ADC1_CHANNEL_7
#define ADC_RESOLUTION ADC_WIDTH_BIT_12
#define ADC_ATTENUATION ADC_ATTEN_DB_6

#endif //WEMOS_LOLIN32

// Use RTC RAM to store Variables that should survive DeepSleep
// =============================================================
// ATTN: define KEEP_RTC_SLOWMEM or vars will be lost (PowerDomain disabled)
//#define KEEP_RTC_SLOWMEM

#ifdef KEEP_RTC_SLOWMEM
extern RTC_DATA_ATTR int SaveMe;
#endif

#endif //HARDWARE_CONFIG_H