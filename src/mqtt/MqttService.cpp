#include "MqttService.h"
#include "../h360/h360service.h"

MqttService::MqttService() : mqttClient(wifiClient) {}

void MqttService::begin(const String &h, int p, const String &u, const String &pw)
{
    host = h;
    port = p;
    user = u;
    pass = pw;

    mqttClient.setServer(host.c_str(), port);

    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int len)
                           { this->handleConfigMessage(topic, payload, len); });

    reconnect();
}

void MqttService::reconnect()
{
    if (mqttClient.connected())
        return;

    Serial.println("====== MQTT RECONNECT ======");
    Serial.println("Host: " + host);
    Serial.println("Port: " + String(port));
    Serial.println("User: " + user);

    String clientId = "esp32-" + String(random(0xffff), HEX);

    bool ok = user.length() > 0
                  ? mqttClient.connect(clientId.c_str(), user.c_str(), pass.c_str())
                  : mqttClient.connect(clientId.c_str());

    if (!ok)
    {
        Serial.println("[MQTT] ❌ Error de conexión");
        return;
    }

    Serial.println("[MQTT] ✔ Conectado");

    String topic = "h360/device/" + deviceId + "/config/set";
    Serial.println("[MQTT] Subscribiendo a: " + topic);
    mqttClient.subscribe(topic.c_str());
    Serial.println("[MQTT] ✔ Subscrito");
    Serial.println("============================");
}

void MqttService::loop()
{
    if (!mqttClient.connected())
        reconnect();

    mqttClient.loop();
}

void MqttService::publish(const char *topic, const char *msg)
{
    if (!mqttClient.connected())
        reconnect();

    mqttClient.publish(topic, msg);
}

void MqttService::handleConfigMessage(char *topic, byte *payload, unsigned int len)
{
    payload[len] = '\0';

    Serial.println("⚡ [MQTT] Mensaje recibido en config/set");
    Serial.println("Topic: " + String(topic));
    Serial.println("Payload: " + String((char *)payload));

    H360::applyConfig(String((char *)payload));
}

bool MqttService::isConnected()
{
    return mqttClient.connected();
}
