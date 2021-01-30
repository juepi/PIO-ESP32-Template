/*
 * ESP8266 Template
 * MQTT / OTA specific setup
 */
#include <Arduino.h>
#include "mqtt-ota-config.h"

// Define Variables
char message_buff[20];
#ifdef OTA_UPDATE
bool OTAupdate = false;
bool SentUpdateRequested = false;
bool OtaInProgress = false;
bool OtaIPsetBySketch = false;
bool SentOtaIPtrue = false;
#endif
float VCC = 3.333;
unsigned int SubscribedTopics = 0;
unsigned int ReceivedTopics = 0;