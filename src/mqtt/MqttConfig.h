#pragma once
#include <Arduino.h>
#include <Preferences.h>

class MqttConfig
{
public:
    void load(); // carga desde NVS
    void save(); // guarda en NVS

    

    String host = "";
    int port = 0;
    String user = "";
    String pass = "";

private:
    Preferences prefs;
};
