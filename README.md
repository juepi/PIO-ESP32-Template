# Introduction 
A template for ESP32 programming using VSC + [PlatformIO](https://platformio.org/) supporting MQTT, OTA-flashing, Battery supply voltage readout and ESP Deep-Sleep.  

## Local Requirements
A (local) MQTT broker is mandatory for this firmware.  
Additionally, personal settings like WIFI SSID and Passphrase will be taken from local environment variables, see `platformio.ini`.  

## An important notice on MQTT usage
Until **v1.1.0**, it was neccessary to have **retained messages available for every subscribed topic**. This was due to the main intention of this firmware to run on a MCU that is powered off (sleeping) most of the time. To ensure that the MCU will receive all messages for subscribed topics "instantly" at firmware startup (after sleeping in example) and does not need to wait for someone to publish "latest news", an endless loop was implemented to wait for messages of all subscribed topics (maximizing battery lifetime by minimizing MCU uptime).  
Since **v1.2.0** it is possible to change this behavior in `platformio.ini` with the define switch `WAIT_FOR_SUBSCRIPTIONS`. Do note that you have to ensure that you do not rely on MQTT messages (like configuration data) in your firmware that might not have been received yet.

## Configuration
In addition to the `platformio.ini` file, see header files in the `include/` folder for additional settings.  
Configureable settings (`include/*-config.h`) should be documented well enough there - hopefully *wink*.

### MQTT Topics used
In order to run OTA updates, you will need at least the following MQTT topics on your broker (case sensitive) to be pre-created with the default retained message so ESP can subscribe to them:

* `topic/tree/OTAupdate` - default **retained** Message: **off**  
This will be translated to a bool variable in the sketch. You will need to set the topic value either to "on" or "off". During normal operation, this Topic needs to be set to "off". If you want to run an OTA-update on your ESP, set it to "on" (retained).  
After a successful update, the ESP will reset this flag to "off".

* `topic/tree/OTAinProgress` - default **retained** Message: **off**  
This is a helper flag topic required by the ESP.

* `topic/tree/OTAstatus` - default Message: none  
The ESP will publish OTA status strings here. No need to pre-create this topic, sketch does not subscribe to it (only publish).

* `topic/tree/Vbat` - default Message: none  
The sketch will publish the voltage measured on the configured ADC pin here. Note that we do not subscribe to this topic, we only publish to it.  
**NOTE**: Reading VCC seems to work fine on WEMOS S2 Mini boards with external voltage divider (12k / 3k6) as well as an 100nF ceramtic capacitor attached between the ADC pin and GND. It is also possible to read VCC through a GPIO pin set to output HIGH, the results are the same as reading VCC directly (in my tests). This will help you save power in DeepSleep, so you don't have current running through the ADC voltage divider all the time.

* `topic/tree/SleepUntil` - default Message: none  
If `SLEEP_UNTIL` is enabled in `platformio.ini`, you can send a message containing a unix epoch time value (32bit signed long, encoded in hexadecimal base) to this topic and the ESP will sleep until the set time, supposed that a valid NTP time has been received and the given wake-up-time is in the future. The message may start with "0x", upper and lower case supported.

### Importance of `ClientName` Setting
Note that the `ClientName` configured in the `platformio.ini` file will also be used as the hostname reported to your DHCP server when the ESP fetches an IP-address. This is especially important, as OTA-flashing will also require your networking environment to be able to resolve this hostname to the ESP's IP-address!  
See `upload_port`setting in the `platformio.ini` file. If you're having troubles with OTA-flashing, you might want to check that first by pinging the configured `ClientName`.  

### Compiling and flashing walkthrough
I will give a rough walkthrough on the first steps, assuming you have a working PlatformIO environment:

* Prepare system environment variables with WIFI and MQTT Data:
    - WIFI_SSID
    - WIFI_PSK
    - MQTT_BROKER --> IP-Address of your broker
    - OTA_PWD --> Passphrase to use for OTA updates
* Adopt `ClientName` in `platformio.ini` as needed
* Prepare `platformio.ini` for wired flashing
Deactivate OTA-flashing in the board specific area:
```
;upload_protocol = ${common_env_data.upload_protocol}
;upload_port = ${common_env_data.upload_port}
;upload_flags = ${common_env_data.upload_flags}
```
* Adopt board info (`platformio.ini`) and hardware settings in `include/hardware-config.h` if needed
* Compile and flash

**To re-flash the sketch OTA:**
* Prepare `platformio.ini` for OTA flashing
Activate OTA-flashing in the board specific area of the active board:
```
upload_protocol = ${common_env_data.upload_protocol}
upload_port = ${common_env_data.upload_port}
upload_flags = ${common_env_data.upload_flags}
```
* Set topic `topic/tree/OTAupdate` **retained** "on"
* wait for the ESP to start up (or reset your ESP)
* When the ESP boots, it will slowly blink the onboard LED until all MQTT topics are received
* After that, the onboard LED will start flashing rapidly. The ESP is now waiting for the new binary to be uploaded
* Click "Upload" in PIO to compile and upload the new sketch
* When the upload has finished, the ESP will boot the new sketch and finish the OTA update process by setting `topic/tree/OTAinProgress` and `topic/tree/OTAupdate` to "off".
* You can verify the status by reading the `topic/tree/OTAstatus` topic, which should throw the string "update_success".

### OTA Update and the firewall
Your OTA flashing might fail with the following error:
```
Sending invitation to esp32-test 
Authenticating...OK
20:31:25 [INFO]: Waiting for device...
20:31:35 [ERROR]: No response from device
*** [upload] Error 1
```
If you run Windows, make sure that your local firewall (where you run VSC) allows `C:\Users\your_username\.platformio\python3\python.exe` to freely communicate (Any protocols, any ports - inbound and outbound; use the full path as described, do not use `%USERPROFILE%` environment variable, it won't work!).

### Adding your own code to the template
To add your own functionality to the template, you will need to adopt the following files:  

* `include/mqtt-ota-config.h`  
Update the `TOPTREE` to your needs. If required, adopt `MQTT_MAX_MSG_SIZE` as needed (defaults to 20 bytes). These defines may also be set in `user_config.h`, which will override the settings in this file.   

* `include/user-config.h`  
Define/declare your topics along with required global vars and libs here.  

* `src/user_setup_loop.cpp`  
Add your desired functionality to the `user_loop` and `user_setup` functions.  

* `src/mqtt-subscriptions.cpp`  
Add an array element to the `MqttSubscriptions` struct array, in example:  
```
{.Topic = ota_topic, .Type = 0, .Subscribed = false, .MsgRcvd = 0, .BoolPtr = &OTAupdate }
```
The following data types are supported (parameter `Type`):
* **BOOL (Type = 0)**
Use the `.BoolPtr` to point to a global `bool` variable where you want to store the received messages for this topic. The BOOL type expects either "on" or "off" text messages to be received from the subscribed topic.  
  
* **INT (Type = 1)**
Use the `.IntPtr` to point to a global `int` variable. The String function `.toInt()` will be used to decode the message to an integer value.  
  
* **FLOAT (Type = 2)**
Use the `.FloatPtr` to point to a global `float` variable. The String function `.toFloat()` will be used to decode the message to a float value. Make sure to use dots as decimal point in your messages.  

* **time_t (Type = 3)**
Use the `.TimePtr` to point to a global `time_t` variable, which can hold a Unix epoch timestamp. The function `strtol(msgString.c_str(), NULL, 16)` will be used to decode the message to a `time_t` value. Message will be decoded from a **hexadecimal base**, "0x" prefix optional, upper and lower case supported.  

* **char array / string (Type = 4)**
Use the `.stringPtr` to point to a global `char array` variable. The function `strcp` will be used to directly copy the message payload (extended with a null terminator) to a char arry.
**Attention**: Initialize your char array with `MQTT_MAX_MSG_SIZE`, which is defined in `mqtt-ota-topic.h`.


### Note on delays and MQTT communication
I have tried to get rid of the `delay()`s implemented to allow background WiFi processing (by using `WiFiClient.flush` instead of delays and configuring `WiFiClient.setNoDelay`), but i was not able to get satisfying MQTT communication without them. Either it took too long to fetch new messages from subscribed topics, or sending new messages crashed (resetted) the ESP.  
My recommendation is, to make sure to add at least a 100ms delay after sending a bunch of MQTT messages. You may also want to use the `MqttDelay`function if you add longer delays (above 200ms), as it will automatically call the PubSubClient `loop` function to fetch new MQTT messages every 200ms of delay.  
In addition, i have added a `user_loop` runtime specific delay in the main loop in v1.0.4: if the `user_loop` takes less than 100ms for execution, a 100ms delay will execute in the main loop.

### Behavior on WiFi or MQTT broker connection failures
Since **v1.2.0** it is possible to select the desired behavior of the firmware at network or broker connection errors through `platformio.ini`:
* **Reboot ESP (NET_OUTAGE=0)**
This was the behavior until v1.1.0. Reboot ESP until the network or broker connection failure recovers.  

* **Run locally (NET_OUTAGE=1)**
This will keep the firmware running even without a network connection. It will automatically try to recover the connection to the MQTT broker every `NET_RECONNECT_INTERVAL` (configurable in `wifi_config.h`, defaults to 1 minute). The firmware will also boot without a broker being available, thus you have to make sure that you have **suitable default values programmed into your firmware wherever needed**.


# Version History

## Release v1.0.0
Initial Release

## Release v1.0.1
- Major code cleanup

## Release v1.0.2
- Moved user specific stuff into dedicated files / functions (`user_setup` and `user_loop`)
- Disabled VCC readouts for WEMOS S2 Mini board
- Added description for wired programming of WEMOS S2 Mini in `platformio.ini`

## Release v1.0.3
- Added `MqttDelay` function which handles MQTT connection/subscriptions while delaying
- Some minor changes which may improve OTA-update handling in some cases
- README update on `ClientName` limitation (no dashes allowed)

## Release v1.0.4
- Fixed `ClientName` limitation
- Added ESP reboot when cancelling OTA Update
- Added `user_loop` runtime dependent 100ms delay in main loop (for WiFi background task handling)

## Release v1.1.0
- Reworked MQTT subscriptions
- OTA updating support now mandatory

## Release v1.2.0
- Added selectable firmware behavior on network and/or MQTT broker outages
- Added define switch in `platformio.ini` to allow not to wait for incoming messages on all subscribed topics at firmware boot

## Release v1.3.0
- Added optional SNTP client support (enable in `platformio.ini`, configure in `time-config.h`)
- Added optional support to sleep until a given epoch time (enable in `platformio.ini`)
- Added option to measure DeepSleep time to decrease clock skew during sleep (enable in `time-config.h`)
- Extended MQTT-Subscriptions with type `time_t`
- Added option to use the more precise 8MHz/256 clock for the RTC, which should decrease time-drifts during DeepSleep (increases DeepSleep current for 5-20µA however)
- `user_loop` runtime dependent delay now configurable in `platformio.ini`
- Enable / Tested VCC readouts on ADC1_2 (GPIO3) for S2-Mini boards (Tested with [ESP-Mini-Base](https://github.com/juepi/ESP-Mini-Base))
- Added option to start with WiFi disabled (enable in `platformio.ini`)
- Added `wifi_up()` and `wifi_down()` functions which handle everything to bring networking/MQTT/NTP/OTA-flashing up and down

## Release v1.3.1
- Extended MQTT-Subscriptions with type `string` (char array)
- Switched `MsgRcvd` from type `bool` to `uint32_t` to be able to keep track if new messages arrive for subscribed topics (increases on new message arrival)
- Minor bugfixes