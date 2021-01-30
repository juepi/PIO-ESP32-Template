# Introduction 
A template for ESP8266 programming using VSC + [PlatformIO](https://platformio.org/) supporting MQTT, OTA-flashing, ESP Deep-Sleep and VCC readouts.  

## Local Requirements
A (local) MQTT broker is mandatory for OTA-Flashing. With deactivated OTA-flashing, you might remove MQTT functionality.  
Additionally, personal settings like WIFI SSID and Passphrase will be taken from local environment variables, see `platformio.ini`.  

## Hardware Requirements
To be able to use DEEP_SLEEP functionality, you will most probably need a small hardware modification for you ESP board: connect pin D0 to RST pin. This will allow the ESP to wake up after the defined sleep time as defined in the `include/generic-config.h` file.  

## An important notice on MQTT usage
As the main intention of this program is to run on a MCU that is powered off (sleeping) most of the time, we will **only work with retained messages** here! This ensures that a client subscribing to a topic will receive the last value published "instantly" and does not need to wait for someone to publish "latest news".  
**ATTENTION**: The current version **requires** retained messages for **all topics you subscribe to!** Not having retained message for a subscribed topic will lead to an endless loop until a message is being received. This behavior has been set up to speed up receiving messages for all subscribed topics after initiating the connection to the MQTT broker.

## Configuration
In addition to the `platformio.ini` file, see header files in the `include/` folder for additional settings.  
Configureable settings should be documented well enough there - hopefully ;-)

### MQTT Topics used
In order to run OTA updates, you will need at least the following MQTT topics on your broker (case sensitive) to be pre-created with the default retained message so ESP can subscribe to them:

* `topic/tree/OTAupdate` - default retained Message: **off**  
This will be translated to a bool variable in the sketch. You will need to set the topic value either to "on" or "off". During normal operation, this Topic needs to be set to "off". If you want to run an OTA-update on your ESP, set it to "on" (retained).  
After a successful update, the ESP will reset this flag to "off".

* `topic/tree/OTAinProgress` - default retained Message: **off**  
This is a helper flag topic required by the ESP.

* `topic/tree/OTAstatus` - default Message: none  
The ESP will publish OTA status strings here. No need to pre-create this topic, sketch does not subscribe to it (only publish).

* `topic/tree/Vcc` - default Message: none  
The sketch will publish the voltage measured on the 3.3V supply here. Note that the accuracy is quite low, but it is good enough to detect if the battery is running low when you supply the ESP in example by a LiFePo4 accumulator directly on the 3.3V pin.  
If you want to improve accuracy, measure the actual voltage with a multimeter and adopt the `VCCCORRDIV` in the `hardware-setup.h` file.  
Note that we do not subscribe to this topic, we only publish to it.

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
* Adopt board info and hardware settings in `include/hardware-config.h` if needed
* Compile and flash

**To re-flash the sketch OTA:**
* Prepare `platformio.ini` for OTA flashing
Activate OTA-flashing in the board specific area:
```
upload_protocol = ${common_env_data.upload_protocol}
upload_port = ${common_env_data.upload_port}
upload_flags = ${common_env_data.upload_flags}
```
* Set topic `topic/tree/OTAupdate` retained "on"
* wait for the ESP to start up (or reset your ESP)
* When the ESP boots, it will slowly blink the onboard LED until all MQTT topics are received
* After that, the onboard LED will start flashing rapidly. The ESP is now waiting for the new binary to be uploaded
* Click "Upload" in PIO to compile and upload the new sketch
* When the upload has finished, the ESP will boot the new sketch and finish the OTA update process by setting `topic/tree/OTAinProgress` and `topic/tree/OTAupdate` to "off".
* You can verify the status by reading the `topic/tree/OTAstatus` topic, which should throw the string "update_success"

### Adding your own MQTT topic subscriptions to the sketch
If you want to add your own MQTT topic subscription, you will need to adopt the following files:  

* `include/mqtt-ota-config.h`  
Define/declare your topic along with required vars here.  

* `src/mqtt-ota-setup.cpp`  
Define initial values of your vars here.  

* `src/common-functions.cpp`  
Include message handling of your topic(s) in the `MqttCallback` function by adding new `else if`´s:  
```
else if (String(topic) == your_defined_topic)
```
  

# Version History

## Initial Release v1.0.0
ATTN: OTA flashing did not work due to an error in macro handling!

## Release 1.0.1
- Fixed error in macro handling
- Speedup receiving messages of subscribed topics