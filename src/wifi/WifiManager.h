#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <secrets.h>

class WifiManager
{
public:
    void begin();
    void save(const String &ssid, const String &pass);
    bool isConnected();
    String getIP();

    bool hasCredentials();
    void setCredentials(const String &s, const String &p);

private:
    void load(); // <── ESTA FUNCIÓN FALTABA EN TU .h

    Preferences prefs;

    // <── ESTAS VARIABLES TAMBIÉN FALTABAN
    String ssid = "";
    String pass = "";
};
