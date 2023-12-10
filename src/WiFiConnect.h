#ifndef WIFICONNECT_H
#define WIFICONNECT_H

#include <Arduino.h>

bool connectToWiFi();
void CheckAndReconnectedToWiFiIfDisconnected();

#endif // WIFICONNECT_H