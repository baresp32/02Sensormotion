#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MqttService
{
public:
    MqttService();

    void begin(const String &host, int port, const String &user, const String &pass);

    void loop();
    void publish(const char *topic, const char *message);
    bool isConnected();

    void reconnect();

    void setCallback(MQTT_CALLBACK_SIGNATURE);

    void handleConfigMessage(char *topic, byte *payload, unsigned int length);


private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    String host;
    int port = 1883;
    String username;
    String password;
    String deviceId = "esp32-01"; // temporal, después lo sacamos de H360
};
