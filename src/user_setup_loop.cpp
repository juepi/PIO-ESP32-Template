/*
 * ESP32 Template
 * ==================
 * User specific function "user_loop" called in main loop
 * User specific funtion "user_setup" called in setup loop
 * Add stuff you want to run here
 */
#include "setup.h"

/*
 * User Setup Loop
 * ========================================================================
 */
void user_setup()
{
  // Add stuff you want to initialize in setup here
  // ===============================================
}

/*
 * User Main Loop
 * ========================================================================
 */
void user_loop()
{
// Add stuff you want to periodically run in main loop here
// =========================================================
#ifdef ONBOARD_LED
  // Toggle LED at each loop
  ToggleLed(LED, 500, 4);
#endif
}