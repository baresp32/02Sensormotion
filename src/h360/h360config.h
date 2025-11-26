#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class H360Config {
public:
    uint32_t version = 1;
    String deviceId = "";
    String location = "unknown";
    String name = "h360 device";

    String wifiSsid = "";
    String wifiPassword = "";
    String wifiFallbackSsid = "";
    String wifiFallbackPassword = "";

    String mqttHost = "";
    int mqttPort = 1883;
    String mqttUser = "";
    String mqttPass = "";

    String toJson() const;
    bool fromJson(const String& json);
};
