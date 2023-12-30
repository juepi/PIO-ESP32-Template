/*
 *   ESP32 Template
 *   Hardware / Board specific Settings
 */
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

//
// WEMOS Lolin32 Board
//
#ifdef WEMOS_LOLIN32
// Onboard LED
#define LED 5
// LED is active low
#define LEDON LOW
#define LEDOFF HIGH

// Pin used for reading VCC (ADC1 channel 7, Pin 35)
// ==================================================
// LOLIN32 uses 100k/27k voltage divider (factor 4.7) on board
// DOCS: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
// and: https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
// BOARD: https://metalab.at/wiki/Wemos_LOLIN32
#define VBAT_ADC_PIN 35
#define VDIV 4.7f
#define ADC_ATTENUATION ADC_0db
#define VFULL_SCALE 1.1f
#define ADC_CHAN ADC1_CHANNEL_7
#define ADC_RESOLUTION 12
#define ADC_MAXVAL 4095
//if READ_THROUGH_GPIO is defined, VCC will be read through the configured GPIO. The GPIO will be configured as output and set HIGH in the hardware_setup routine
// can have an advantage when running on batteries, as the external attenuator only draws current when the ESP is active
#define READ_THROUGH_GPIO 14

#endif // WEMOS_LOLIN32

//
// WEMOS S2 Mini (ESP32S2) Board
//
#ifdef WEMOS_S2MINI
// Onboard LED
#define LED 15
// LED is active high
#define LEDON HIGH
#define LEDOFF LOW

// Pin used for reading VCC (ADC1 channel 2, GPIO3)
// ==================================================
// Documentation: https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/adc.html#about
#define VBAT_ADC_PIN 3
#define VDIV 4.348f //external attenuator on ESP-Mini-Base (https://github.com/juepi/ESP-Mini-Base); use 1 if no external attenuator is used
#define ADC_ATTENUATION ADC_2_5db // measureable voltage @2.5dB: 0..1050mV; adopt to your needs
#define VFULL_SCALE 1.114f // adopt to your hardware to increase accuracy; sadly, adc_vref_to_gpio function seems to be not available in Arduino framework
#define ADC_CHAN ADC1_CHANNEL_2 //GPIO3
#define ADC_RESOLUTION 12 //bits
#define ADC_MAXVAL 4095
//if READ_THROUGH_GPIO is defined, VCC will be read through the configured GPIO. The GPIO will be configured as output and set HIGH in the hardware_setup routine
// can have an advantage when running on batteries, as the external attenuator only draws current when the ESP is active
#define READ_THROUGH_GPIO 14

#endif // WEMOS_S2MINI

#endif // HARDWARE_CONFIG_H