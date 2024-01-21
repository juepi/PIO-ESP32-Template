/*
 *   ESP32 Template
 *   User specific configuration, defines and Function declarations
 */
#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include "mqtt-ota-config.h"

// Define required user libraries here
// Don't forget to add them into platformio.ini
//#define <user_lib.h>

// Declare user setup and main loop functions
extern void user_setup();
extern void user_loop();

// Declare global user specific objects
// extern abc xyz;

//
// Use RTC RAM to store Variables that should survive DeepSleep
//
// ATTN: define KEEP_RTC_SLOWMEM or vars will be lost (PowerDomain disabled)
//#define KEEP_RTC_SLOWMEM

#ifdef KEEP_RTC_SLOWMEM
// Example
extern RTC_DATA_ATTR int SaveMe;
#endif

#endif // USER_CONFIG_H