#ifndef WIFICONNECT_H
#define WIFICONNECT_H

#include <Arduino.h>

bool ConnectToWiFi();
void CheckAndReconnectedToWiFiIfDisconnected();

#endif // WIFICONNECT_H