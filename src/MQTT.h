#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>

void ConnectToMqttBroker();
void PublishMessageToMqttBroker(String message, const char* mqttTopic);
void LoopMqttClientAndReconnectIfDisconnected();

#define STATE_TOPIC "homeassistant/sensor/catlitterodourmotiondetector/state"
#define ODOUR_LEVEL_STRING_TOPIC "homeassistant/sensor/catlitterodourmotiondetector_odour_level_string/config"
#define ODOUR_LEVEL_PPM_TOPIC "homeassistant/sensor/catlitterodourmotiondetector_odour_level/config"
#define MOTION_TOPIC "homeassistant/sensor/catlitterodourmotiondetector_motion/config"

#endif // MQTT_H