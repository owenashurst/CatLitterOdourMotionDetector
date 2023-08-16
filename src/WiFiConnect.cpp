#include <ESP8266WiFi.h>
#include "Config.h"

const char* WifiName = "";
const char* WifiPassword = "";

const int retryLimit = 5;

bool connectToWiFi() {
    WiFi.setHostname("CatLitterDetector");
    WiFi.begin(WifiName, WifiPassword);

    int i = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);

        if (i >= retryLimit) {
            return false;
        }
    }

    return true;
}