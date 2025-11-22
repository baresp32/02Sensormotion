#pragma once
#include "BluetoothSerial.h"
#include "wifi/WifiManager.h"

class BluetoothProvision
{
private:
    BluetoothSerial BT;
    WifiManager *wifi;

public:
    BluetoothProvision(WifiManager *ref);
    void begin();
    void listen();
};
