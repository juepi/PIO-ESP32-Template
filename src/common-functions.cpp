/*
 * ESP8266 Template
 * Common Functions
 */
#include <Arduino.h>
#include "common-functions.h"

void ToggleLed(int PIN, int WaitTime, int Count)
{
    // Toggle digital output
    for (int i = 0; i < Count; i++)
    {
        digitalWrite(PIN, !digitalRead(PIN));
        delay(WaitTime);
    }
}

/*
 * Callback Functions
 * ========================================================================
 */
//MQTT Subscription callback function
void MqttCallback(char *topic, byte *payload, unsigned int length)
{
    unsigned int i = 0;
    // create character buffer with ending null terminator (string)
    for (i = 0; i < length; i++)
    {
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    String msgString = String(message_buff);

    DEBUG_PRINTLN("MQTT: Message arrived [" + String(topic) + "]: " + String(msgString));

// run through topics
#ifdef OTA_UPDATE
    if (String(topic) == ota_topic)
    {
        if (msgString == "on")
        {
            OTAupdate = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            OTAupdate = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid OTA-Update: " + String(msgString));
            delay(200);
        }
    }
    else if (String(topic) == otaInProgress_topic)
    {
        if (msgString == "on")
        {
            OtaInProgress = true;
            ReceivedTopics++;
        }
        else if (msgString == "off")
        {
            OtaInProgress = false;
            ReceivedTopics++;
        }
        else
        {
            DEBUG_PRINTLN("MQTT: ERROR: Fetched invalid OtaInProgress: " + String(msgString));
            delay(200);
        }
    }
    else
    {
        DEBUG_PRINTLN("ERROR: Unknown topic: " + String(topic));
        DEBUG_PRINTLN("ERROR: Unknown topic value: " + String(msgString));
        delay(200);
    }
#endif //OTA_UPDATE
}
