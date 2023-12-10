#include <ESP8266WiFi.h>
#include "Config.h"

const char* WifiName = "";
const char* WifiPassword = "";

bool connectToWiFi() {
    WiFi.setHostname("CatLitterDetector");
    WiFi.begin(WifiName, WifiPassword);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    Serial.println("Connected to Wi-Fi.");
    Serial.println("IP Address: " + WiFi.localIP().toString());

    return true;
}

void CheckAndReconnectedToWiFiIfDisconnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi connection lost. Reconnecting...");

        WiFi.disconnect();
        WiFi.begin(WifiName, WifiPassword);

        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        }

        Serial.println("Reconnected to Wi-Fi.");
    }
}