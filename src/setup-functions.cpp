/*
 * ESP8266 Template
 * Setup Functions
 */
#include <Arduino.h>
#include "setup-functions.h"

void wifi_setup()
{
    // Set WiFi Sleep Mode
    WiFi.setSleepMode(WIFISLEEP);

    // Set WiFi Hostname
    WiFi.hostname(WIFI_DHCPNAME);

    // Connect to WiFi network
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Connecting to " + String(ssid));
    WiFi.begin(ssid, password);
    // Next command avoids ESP broadcasting an unwanted ESSID..
    WiFi.mode(WIFI_STA);
    unsigned long end_connect = millis() + WIFI_CONNECT_TIMEOUT;
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() >= end_connect)
        {
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("Failed to connect to " + String(ssid));
#ifdef ONBOARD_LED
            ToggleLed(LED, 1000, 4);
#endif
#ifdef DEEP_SLEEP
            ESP.deepSleep(DS_DURATION_MIN * 60000000);
            delay(3000);
#else
            ESP.reset();
#endif
        }
        delay(500);
        DEBUG_PRINT(".");
    }
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi connected");
    DEBUG_PRINT("Device IP Address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    DEBUG_PRINT("DHCP Hostname: ");
    DEBUG_PRINTLN(WIFI_DHCPNAME);
#ifdef ONBOARD_LED
    // WiFi connected - blink once
    ToggleLed(LED, 200, 2);
#endif
}

#ifdef OTA_UPDATE
void ota_setup()
{
    // Setup OTA Updates
    //ATTENTION: calling MQTT Publish function inside ArduinoOTA functions MIGHT NOT WORK!
    ArduinoOTA.setHostname(OTA_CLTNAME);
    ArduinoOTA.setPassword(OTA_PWD);
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_SPIFFS
            type = "filesystem";
        }
    });
    ArduinoOTA.onEnd([]() {
#ifdef ONBOARD_LED
        ToggleLed(LED, 200, 4);
#else
        //ATTENTION: calling MQTT Publish function here does NOT WORK!
        delay(200);
#endif
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        int percentComplete = (progress / (total / 100));
        if (percentComplete == 100)
        {
            DEBUG_PRINTLN("Upload complete.");
            delay(500);
        }
    });
    ArduinoOTA.onError([](ota_error_t error) {
        DEBUG_PRINTLN("Error: " + String(error));
        delay(500);
    });
    ArduinoOTA.begin();
}
#endif