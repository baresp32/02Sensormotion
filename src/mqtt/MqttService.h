#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MqttService
{
public:
    MqttService();

    void begin(const String &host, int port, const String &user, const String &pass);
    void reconnect();
    void loop();
    void publish(const char *topic, const char *msg);

    bool isConnected();


private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    String host;
    int port = 1883;
    String user;
    String pass;

    String deviceId = "esp32-01";  
    void handleConfigMessage(char *topic, byte *payload, unsigned int length);
};
