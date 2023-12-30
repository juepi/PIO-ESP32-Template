/*
 * ESP32 Template
 * ==================
 * User specific function "user_loop" called in main loop
 * User specific funtion "user_setup" called in setup loop
 * Add stuff you want to run here
 */
#include "setup.h"

// Variables that should be saved during DeepSleep
#ifdef KEEP_RTC_SLOWMEM
RTC_DATA_ATTR int SaveMe = 0;
#endif

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
  ToggleLed(LED, 1, 1);
#endif
#ifdef NTP_CLT
  DEBUG_PRINTLN(&TimeInfo, "%A, %B %d %Y %H:%M:%S");
#else
  DEBUG_PRINTLN("user_loop finished.");
#endif
  MqttDelay(1000);
}