/*
 *   ESP32 Template
 *   User specific defines and Function declarations
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

#endif // USER_CONFIG_H