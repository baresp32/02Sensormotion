#pragma once
#include "BluetoothSerial.h"
#include "wifi/WifiManager.h"

// Declara SerialBT globalmente
extern BluetoothSerial SerialBT;

class BluetoothProvision
{
private:
    WifiManager *wifi;
    bool active = false; // <-- IMPORTANTE para saber si BT está activo

public:
    BluetoothProvision(WifiManager *ref);
    void begin();
    void listen();
    void stop();
    bool isActive() const { return active; }
};
