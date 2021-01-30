/*
 * ESP32 Template
 * Hardware specific setup
 */
#include <Arduino.h>
#include "hardware-config.h"

// Variables that should be saved during DeepSleep
#ifdef KEEP_RTC_SLOWMEM
RTC_DATA_ATTR int SaveMe = 0;
#endif