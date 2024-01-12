/*
 * ESP32 Template
 * ==================
 *
 * Includes useful functions like
 * - DeepSleep
 * - MQTT
 * - OTA Updates (ATTN: requires MQTT!)
 *
 */
#include <setup.h>

/*
 * Main Loop
 * ========================================================================
 */
void loop()
{
#ifdef WIFI_DELAY
  static unsigned long start_user_loop = 0;
  static unsigned long duration_user_loop = 0;
#endif
  static unsigned long netfail_reconn_millis = 0;

//
// Handle local tasks
//
#ifdef READVCC
  // Read VCC every 10 seconds
  // AD conversion draws quite some power, so don't run too often
  static unsigned long Next_VCC_ADC = 0;
  if (millis() > Next_VCC_ADC)
  {
    VCC = VDIV * VFULL_SCALE * float(analogRead(VBAT_ADC_PIN)) / ADC_MAXVAL;
    Next_VCC_ADC = millis() + 10000;
  }
#endif

  //
  // Handle network / MQTT broker connection failures (NET_OUTAGE=1)
  //
  if (NetState == NET_FAIL)
  {
    // Currently no network available, try to recover
    if (millis() >= netfail_reconn_millis)
    {
      if (WiFi.isConnected())
      {
        // Try to reconnect to Broker
        MqttUpdater();
      }
      if (NetState == NET_FAIL)
      {
        // Still no network available, try to recover every NET_RECONNECT_INTERVAL
        netfail_reconn_millis = millis() + NET_RECONNECT_INTERVAL;
      }
      else
      {
        // Recovered from network outage
        NetRecoveryMillis = millis();
        netfail_reconn_millis = 0;
      }
    }
  }

  //
  // Handle network tasks
  //
  if (NetState == NET_UP)
  {
    // Check connection to MQTT broker, subscribe and update topics
    MqttUpdater();

    // Handle OTA updates
    if (OTAUpdateHandler())
    {
      // OTA Update in progress, restart main loop
      return;
    }
#ifdef READVCC
    // Publish VCC to MQTT
    static unsigned long Next_Mqtt_Publish = 0;
    if (millis() >= Next_Mqtt_Publish)
    {
      mqttClt.publish(vcc_topic, String(VCC).c_str(), true);
      delay(150);
      Next_Mqtt_Publish = millis() + (MQTT_PUB_INTERVAL * 1000);
      DEBUG_PRINTLN("VCC = " + String(VCC) + " V");
    }
#endif
#ifdef NTP_CLT
    // Update time variables with a timeout of 200ms
    getLocalTime(&TimeInfo, 200);
    time(&EpochTime);
    if (EpochTime < 1704300000)
    {
      // System time is in the past, somethings wrong - retry
      getLocalTime(&TimeInfo, 200);
      time(&EpochTime);
      if (EpochTime < 1704300000)
      {
        DEBUG_PRINTLN("Wrong system time, invalidating local time and retry in next loop");
        NTPSyncCounter = 0;
      }
    }
#endif
#ifdef MEASURE_SLEEP_CLOCK_SKEW
    static bool SkewDataSent = false;
    if (esp_reset_reason() == ESP_RST_DEEPSLEEP)
    {
      // Woke up from deepsleep, output some helpers to allow calculation of SLEEPT_CORR_FACT
      if (NTPSyncCounter > 0 && !SkewDataSent)
      {
        mqttClt.publish(boot_dur_topic, String(millis()).c_str(), false);
        mqttClt.publish(end_sleep_topic, String(EpochTime).c_str(), false);
        SkewDataSent = true;
      }
    }
#endif
  }

//
// Handle user_loop
//
#ifdef WIFI_DELAY
  // Run user specific loop and measure duration
  start_user_loop = millis();
  user_loop();
  duration_user_loop = millis() - start_user_loop;

  // Spare some CPU time for background tasks (if we're not in a hurry)
  if (duration_user_loop < 100)
  {
    delay(WIFI_DELAY);
  }
#else
  user_loop();
#endif

//
// Handle SleepUntil
//
#ifdef SLEEP_UNTIL
  if (NTPSyncCounter > 0 && EpochTime < SleepUntilEpoch)
  {
    time(&EpochTime);
    // System time synced and received sleep-until time in the future -> OK!
    // calculate time to sleep in µs
    uint64_t WakeAfter_us = (((uint64_t)SleepUntilEpoch - (uint64_t)EpochTime) * 1000000ULL);
#ifdef MEASURE_SLEEP_CLOCK_SKEW
    DEBUG_PRINTLN("Configured Sleep time in seconds: " + String(SleepUntilEpoch - EpochTime));
    DEBUG_PRINTLN("Epoch at start sleep: " + String(EpochTime));
    mqttClt.publish(set_sleep_dur_topic, String(SleepUntilEpoch - EpochTime).c_str(), false);
    mqttClt.publish(start_sleep_topic, String(EpochTime).c_str(), false);
    delay(100);
#endif
    wifi_down();
    esp_deep_sleep(WakeAfter_us);
  }
#endif

//
// Handle DeepSleep
//
#ifdef E32_DEEP_SLEEP
  // disconnect WiFi and go to sleep
  DEBUG_PRINTLN("Good night for " + String(DS_DURATION_MIN) + " minutes.");
  wifi_down();
  esp_deep_sleep((uint64_t)DS_DURATION_MIN * 60000000);
#endif

  // First iteration of main loop finished
  JustBooted = false;
}