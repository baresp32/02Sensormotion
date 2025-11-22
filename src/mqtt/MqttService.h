#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MqttService
{
public:
    MqttService();

    void begin();
    void loop();
    void publish(const char *topic, const char *message);
    bool isConnected();

    void reconnect();

private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
};
