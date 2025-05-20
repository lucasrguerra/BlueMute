#ifndef WI_FI_H
#define WI_FI_H

#include <Arduino.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <vector>
#include <set>

class WiFi {
public:
    WiFi();
    void begin();
    void startJamming();
    void stopJamming();
    void performScan();
    void executeJamming();
    int findedAPs();
};

#endif