#include <ESP8266WiFi.h>
#include "Config.h"

const char* WifiName = "";
const char* WifiPassword = "";

bool ConnectToWiFi() {
    WiFi.setHostname("CatLitterDetector");
    WiFi.begin(WifiName, WifiPassword);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Waiting 5 seconds...");
        delay(5000);
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
            delay(5000);
        }

        Serial.println("Reconnected to Wi-Fi.");
    }
}