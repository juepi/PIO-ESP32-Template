/*
 * ESP8266 Template
 * Hardware specific setup
 */
#include <Arduino.h>
#include "hardware-config.h"

// Set ADC mode to read VCC (Attn: Pin A0 must be floating!)
// ==========================================================
ADC_MODE(ADC_VCC);
