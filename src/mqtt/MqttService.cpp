#include "MqttService.h"
#include "../h360/h360service.h" // Para llamar H360::applyConfig

MqttService::MqttService() : mqttClient(wifiClient) {}

// ========================================================
//  BEGIN con parámetros dinámicos (H360)
// ========================================================
void MqttService::begin(const String &host, int port, const String &user, const String &pass)
{
    this->host = host;
    this->port = port;
    this->username = user;
    this->password = pass;

    mqttClient.setServer(host.c_str(), port);

    // Registrar callback MQTT → MqttService::handleConfigMessage()
    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                           { this->handleConfigMessage(topic, payload, length); });

    reconnect();
}

// ========================================================
//  ESTADO
// ========================================================
bool MqttService::isConnected()
{
    return mqttClient.connected();
}

// ========================================================
//  RECONNECT
// ========================================================
void MqttService::reconnect()
{
    if (mqttClient.connected())
        return;

    Serial.print("[MQTT] Intentando conectar... ");

    String clientId = "esp32-" + String(random(0xffff), HEX);
    bool connected;

    if (username.length() > 0)
    {
        connected = mqttClient.connect(clientId.c_str(), username.c_str(), password.c_str());
    }
    else
    {
        connected = mqttClient.connect(clientId.c_str());
    }

    if (connected)
    {
        Serial.println("Conectado!");

        // Suscribir comandos clásicos
        mqttClient.subscribe("esp32/cmd/#");

        // Suscribir config industrial
        String topic = "h360/device/" + deviceId + "/config/set";
        mqttClient.subscribe(topic.c_str());
        Serial.print("[MQTT] Subscrito: ");
        Serial.println(topic);
    }
    else
    {
        Serial.print("Fallo, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" Reintentando en 3s...");
        delay(3000);
    }
}

// ========================================================
//  PUBLISH
// ========================================================
void MqttService::publish(const char *topic, const char *message)
{
    if (!mqttClient.connected())
        reconnect();

    mqttClient.publish(topic, message);
}

// ========================================================
//  LOOP
// ========================================================
void MqttService::loop()
{
    if (!mqttClient.connected())
        reconnect();

    mqttClient.loop();
}

// ========================================================
//  CALLBACK MQTT → Manejo de config industrial
// ========================================================
void MqttService::handleConfigMessage(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0';

    Serial.print("[MQTT] Config recibida: ");
    Serial.println(topic);

    String json = String((char *)payload);

    // Llamar a H360 para actualizar configuración
    H360::applyConfig(json);
}
