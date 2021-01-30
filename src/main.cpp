/*
 * ESP8266 Template
 * ==================
 * 
 * Includes useful functions like
 * - DeepSleep
 * - Read VCC
 * - MQTT
 * - OTA Updates (ATTN: requires MQTT!)
 * 
 * If you use DeepSleep, make sure to connect pin 16 (D0) to RESET, or your ESP will never wake up!
 * Also keep in mind that you can DeepSleep for ~ 1 hour max (hardware limitation)!
 * ATTENTION: Keep in mind that it takes quite a while after the sketch has booted until we receive messages from all subscribed topics!
 * In my case it takes up to 27 seconds (see WAIT_MILLIS_FOR_TOPICS_AFTER_BOOT).
 * To get OTA update working on windows + Arduino-IDE, you need to install python and python.exe needs to be in %PATH%
 * First flash needs to be wired of course. Afterwards Arduino IDE needs to be restarted if you cannot find
 * the ESP OTA-port in the IDE (also MQTT ota_topic needs to be set to "on" to be able to flash OTA).
 * Keep in mind that you'll need a reliable power source for OTA updates, 2x AA batteries might not work.
 * If you brick your ESP during OTA update, you can probably revive it by flashing it wired.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "hardware-config.h"
#include "generic-config.h"
#include "wifi-config.h"
#include "mqtt-ota-config.h"
#include "common-functions.h"
#include "setup-functions.h"

// Setup WiFi instance
WiFiClient WIFI_CLTNAME;

// Setup PubSub Client instance
PubSubClient mqttClt(MQTT_BROKER, 1883, MqttCallback, WIFI_CLTNAME);

/*
/ Functions
*/

// Function to subscribe to MQTT topics
bool MqttSubscribe(const char *Topic)
{
  if (mqttClt.subscribe(Topic))
  {
    DEBUG_PRINTLN("Subscribed to " + String(Topic));
    SubscribedTopics++;
    mqttClt.loop();
    return true;
  }
  else
  {
    DEBUG_PRINTLN("Failed to subscribe to " + String(Topic));
    delay(100);
    return false;
  }
}

// Function to connect to MQTT Broker and subscribe to Topics
bool ConnectToBroker()
{
  // Reset subscribed/received Topics counters
  SubscribedTopics = 0;
  ReceivedTopics = 0;
  bool RetVal = false;
  int ConnAttempt = 0;
  // Try to connect x times, then return error
  while (ConnAttempt < MAXCONNATTEMPTS)
  {
    DEBUG_PRINT("Connecting to MQTT broker..");
    // Attempt to connect
    if (mqttClt.connect(MQTT_CLTNAME))
    {
      DEBUG_PRINTLN("connected");
      RetVal = true;

// Subscribe to Topics
#ifdef OTA_UPDATE
      MqttSubscribe(ota_topic);
      MqttSubscribe(otaInProgress_topic);
#endif //OTA_UPDATE
      delay(200);
      break;
    }
    else
    {
      DEBUG_PRINTLN("failed, rc=" + String(mqttClt.state()));
      DEBUG_PRINTLN("Sleeping 2 seconds..");
      delay(2000);
      ConnAttempt++;
    }
  }
  return RetVal;
}

/*
 * Setup
 * ========================================================================
 */
void setup()
{
// start serial port and digital Outputs
#ifdef SERIAL_OUT
  Serial.begin(BAUD_RATE);
#endif
  DEBUG_PRINTLN();
  DEBUG_PRINTLN(FIRMWARE_NAME);
  DEBUG_PRINTLN(FIRMWARE_VERSION);
#ifdef ONBOARD_LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LEDOFF);
#endif

  // Startup WiFi
  wifi_setup();

  // Setup MQTT Connection to broker and subscribe to topic
  if (ConnectToBroker())
  {
    // New connection to broker, fetch topics
    // ATTN: will run endlessly if subscribed topics
    // does not have retained messages and no one posts a message
    DEBUG_PRINT("Waiting for topics..");
    while (ReceivedTopics < SubscribedTopics)
    {
      DEBUG_PRINT(".");
      mqttClt.loop();
#ifdef ONBOARD_LED
      ToggleLed(LED, 100, 2);
#else
      delay(100);
#endif
    }
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("All topics received.");
  }
  else
  {
    DEBUG_PRINTLN("Unable to connect to MQTT broker.");
#ifdef ONBOARD_LED
    ToggleLed(LED, 100, 40);
#endif
#ifdef DEEP_SLEEP
    ESP.deepSleep(DS_DURATION_MIN * 60000000);
    delay(3000);
#else
    ESP.reset();
#endif
    delay(1000);
  }

  // Setup OTA
#ifdef OTA_UPDATE
  ota_setup();
#endif

#ifdef ONBOARD_LED
  // Signal setup finished
  ToggleLed(LED, 200, 6);
#endif
}

/*
 * Main Loop
 * ========================================================================
 */
void loop()
{
  delay(200);
  // Check connection to MQTT broker and update topics
  if (!mqttClt.connected())
  {
    if (ConnectToBroker())
    {
      // New connection to broker, fetch topics
      // ATTN: will run endlessly if subscribed topics
      // does not have retained messages and no one posts a message
      DEBUG_PRINT("Waiting for topics..");
      while (ReceivedTopics < SubscribedTopics)
      {
        DEBUG_PRINT(".");
        mqttClt.loop();
#ifdef ONBOARD_LED
        ToggleLed(LED, 100, 2);
#else
        delay(100);
#endif
      }
      DEBUG_PRINTLN("");
      DEBUG_PRINTLN("All topics received.");
    }
    else
    {
      DEBUG_PRINTLN("Unable to connect to MQTT broker.");
#ifdef ONBOARD_LED
      ToggleLed(LED, 100, 40);
#endif
#ifdef DEEP_SLEEP
      ESP.deepSleep(DS_DURATION_MIN * 60000000);
      delay(3000);
#else
      ESP.reset();
#endif
    }
  }
  else
  {
    mqttClt.loop();
  }

#ifdef OTA_UPDATE
  // If OTA Firmware Update is requested,
  // only loop through OTA function until finished (or reset by MQTT)
  if (OTAupdate)
  {
    if (OtaInProgress && !OtaIPsetBySketch)
    {
      DEBUG_PRINTLN("OTA firmware update successful, resuming normal operation..");
      mqttClt.publish(otaStatus_topic, String(UPDATEOK).c_str(), true);
      mqttClt.publish(ota_topic, String("off").c_str(), true);
      mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
      OTAupdate = false;
      OtaInProgress = false;
      OtaIPsetBySketch = true;
      SentOtaIPtrue = false;
      SentUpdateRequested = false;
      return;
    }
    if (!SentUpdateRequested)
    {
      mqttClt.publish(otaStatus_topic, String(UPDATEREQ).c_str(), true);
      SentUpdateRequested = true;
    }
    DEBUG_PRINTLN("OTA firmware update requested, waiting for upload..");
#ifdef ONBOARD_LED
    // Signal OTA update requested
    ToggleLed(LED, 100, 10);
#endif
    //set MQTT reminder that OTA update was executed
    if (!SentOtaIPtrue)
    {
      DEBUG_PRINTLN("Setting MQTT OTA-update reminder flag on broker..");
      mqttClt.publish(otaInProgress_topic, String("on").c_str(), true);
      OtaInProgress = true;
      SentOtaIPtrue = true;
      OtaIPsetBySketch = true;
      delay(100);
    }
    //call OTA function to receive upload
    ArduinoOTA.handle();
    return;
  }
  else
  {
    if (SentUpdateRequested)
    {
      DEBUG_PRINTLN("OTA firmware update cancelled by MQTT, resuming normal operation..");
      mqttClt.publish(otaStatus_topic, String(UPDATECANC).c_str(), true);
      mqttClt.publish(otaInProgress_topic, String("off").c_str(), true);
      OtaInProgress = false;
      OtaIPsetBySketch = true;
      SentOtaIPtrue = false;
      SentUpdateRequested = false;
    }
  }
#endif

// START STUFF YOU WANT TO RUN HERE!
// ============================================
#ifdef ONBOARD_LED
  // Toggle LED at each loop
  ToggleLed(LED, 500, 4);
#endif

  // Read VCC and publish to MQTT
  delay(300);
  VCC = ((float)ESP.getVcc()) / VCCCORRDIV;
  mqttClt.publish(vcc_topic, String(VCC).c_str(), true);
  delay(100);

#ifdef DEEP_SLEEP
  // disconnect WiFi and go to sleep
  DEBUG_PRINTLN("Good night for " + String(DS_DURATION_MIN) + " minutes.");
  WiFi.disconnect();
  ESP.deepSleep(DS_DURATION_MIN * 60000000);
#else
  DEBUG_PRINTLN("Loop finished, DeepSleep disabled. Restarting in 5 seconds.");
#endif
  //ATTN: Sketch continues to run for a short time after initiating DeepSleep, so pause here
  delay(5000);
}