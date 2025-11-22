#include "MqttService.h"
#include "secrets.h"

MqttService::MqttService() : mqttClient(wifiClient) {}

void MqttService::begin()
{
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    Serial.println("[MQTT] Configurado.");
}

bool MqttService::isConnected()
{
    return mqttClient.connected();
}

void MqttService::reconnect()
{
    if (mqttClient.connected())
        return;

    Serial.print("[MQTT] Intentando conectar... ");

    String clientId = "esp32-" + String(random(0xffff), HEX);

    bool connected;

#if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
    connected = mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
#else
    connected = mqttClient.connect(clientId.c_str());
#endif

    if (connected)
    {
        Serial.println("Conectado!");
        mqttClient.subscribe("esp32/cmd/#");
    }
    else
    {
        Serial.print("Fallo, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" Reintentando en 3s...");
        delay(3000);
    }
}

void MqttService::publish(const char *topic, const char *message)
{
    if (!mqttClient.connected())
        reconnect();

    mqttClient.publish(topic, message);
}

void MqttService::loop()
{
    if (!mqttClient.connected())
    {
        reconnect();
    }
    mqttClient.loop();
}
