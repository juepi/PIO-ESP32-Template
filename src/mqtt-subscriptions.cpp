/*
 * ESP32 Template
 * Definition of MQTT subscriptions
 */
#include "mqtt-ota-config.h"
#include "user-config.h"

//
// MqttSubscriptions is a dataset with all configuration information required
// to subscribe to configured topics and handle the received messages
// Results of decoded messages will be stored into a configured global variable using a pointer
//
// Configuration:
// .Topic: String of topic to subscribe to
// .Type:   0 = bool (expected message "on" or "off")
//          1 = integer (int)
//          2 = float
//          3 = time_t (decoded as hex! message may start with "0x", upper/lower chars supported)
// .Subscribed: flag, true if successfully subscribed to topic
// .MsgRcvd: flag, true if a message has been received for subscribed topic
// .[Bool|Int|Float]Ptr: Pointer to a global var (according to "Type") where the decoded message info will be stored 
//

#ifdef SLEEP_UNTIL
const int SubscribedTopicCnt = 3; // Overall amount of topics to subscribe to
#else
const int SubscribedTopicCnt = 2; // Overall amount of topics to subscribe to
#endif

MqttSubCfg MqttSubscriptions[SubscribedTopicCnt]={
#ifdef SLEEP_UNTIL
    {.Topic = sleep_until_topic, .Type = 3, .Subscribed = false, .MsgRcvd = false, .TimePtr = &SleepUntilEpoch },
#endif
    {.Topic = ota_topic, .Type = 0, .Subscribed = false, .MsgRcvd = false, .BoolPtr = &OTAupdate },
    {.Topic = otaInProgress_topic, .Type = 0, .Subscribed = false, .MsgRcvd = false, .BoolPtr = &OtaInProgress }
};
