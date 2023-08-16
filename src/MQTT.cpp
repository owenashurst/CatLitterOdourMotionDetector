#include <Arduino_JSON.h>
#include <PicoMQTT.h>

#include "MQTT.h"

const char* mqtt_server = "192.168.0.100";
const int mqtt_server_port = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

PicoMQTT::Client mqtt;

void CreateOdourLevelStringSensorConfig() {
    JSONVar odourStringSensorConfig;

    odourStringSensorConfig["name"] = "Odour Level String";
    odourStringSensorConfig["unique_id"] = "odour_level_string";
    odourStringSensorConfig["state_topic"] = STATE_TOPIC;
    odourStringSensorConfig["value_template"] = "{{ value_json.odourLevel }}";
    odourStringSensorConfig["icon"] = "mdi:air-filter";
    
    JSONVar device;
    device["name"] = "Cat Litter Odour Motion Detector";
    device["identifiers"] = "[\"cat_litter_odour_motion_detector\"]";
    odourStringSensorConfig["device"] = device;

    PublishMessageToMqttBroker(JSON.stringify(odourStringSensorConfig), ODOUR_LEVEL_STRING_TOPIC);

    Serial.println("Odour Level String Sensor Config Created");
}

void CreateOdourLevelSensorConfig() {
    JSONVar odourLevelSensorConfig;

    odourLevelSensorConfig["name"] = "Odour Level";
    odourLevelSensorConfig["unique_id"] = "odour_level";
    odourLevelSensorConfig["state_topic"] = STATE_TOPIC;
    odourLevelSensorConfig["state_class"] = "measurement";
    odourLevelSensorConfig["device_class"] = "volatile_organic_compounds_parts";
    odourLevelSensorConfig["unit_of_measurement"] = "ppm";
    odourLevelSensorConfig["value_template"] = "{{ value_json.odourLevelPpm }}";
    odourLevelSensorConfig["icon"] = "mdi:air-filter";

    JSONVar device;
    device["name"] = "Cat Litter Odour Motion Detector";
    device["identifiers"] = "[\"cat_litter_odour_motion_detector\"]";
    odourLevelSensorConfig["device"] = device;

    PublishMessageToMqttBroker(JSON.stringify(odourLevelSensorConfig), ODOUR_LEVEL_PPM_TOPIC);

    Serial.println("Odour Level Sensor Config Created");
}

void CreateMotionSensorConfig() {
    JSONVar motionSensorConfig;

    motionSensorConfig["name"] = "Motion";
    motionSensorConfig["unique_id"] = "motion";
    motionSensorConfig["state_topic"] = STATE_TOPIC;
    motionSensorConfig["value_template"] = "{{ value_json.motion }}";
    motionSensorConfig["icon"] = "mdi:cat";

    JSONVar device;
    device["name"] = "Cat Litter Odour Motion Detector";
    device["identifiers"] = "[\"cat_litter_odour_motion_detector\"]";
    motionSensorConfig["device"] = device;

    PublishMessageToMqttBroker(JSON.stringify(motionSensorConfig), MOTION_TOPIC);

    Serial.println("Motion Sensor Config Created");
}

void ConnectToMqttBroker() {
    while (!mqtt.connected()) {
        Serial.println("Attempting MQTT connection...");
        String mqttClientId = "CatOdourDetector";
        if (mqtt.connect(mqtt_server, mqtt_server_port, mqttClientId.c_str(), mqttUser, mqttPassword)) {
            Serial.println("Connected to MQTT broker.");
            CreateOdourLevelStringSensorConfig();
            CreateOdourLevelSensorConfig();
            CreateMotionSensorConfig();
        } else {
            Serial.println("Connection to MQTT broker failed. Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void PublishMessageToMqttBroker(String message, const char* mqttTopic) {
    mqtt.publish(mqttTopic, message.c_str(), 0, true);

    Serial.println("Message published to MQTT broker. Message: "+ message);
}

void LoopMqttClient() {
    mqtt.loop();
}