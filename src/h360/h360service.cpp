#include "h360service.h"
#include "../mqtt/MqttService.h" // Necesario para publicar ACK
#include <ArduinoJson.h>

static H360Config config;
static H360Storage storage;

// Declarado extern en MqttService.cpp
extern MqttService mqtt;

namespace H360
{
    void setup()
    {
        Serial.println("[h360] inicializando storage...");
        storage.begin();

        if (storage.loadConfig(config))
        {
            Serial.println("[h360] config cargada desde nvs:");
            Serial.println(config.toJson());
        }
        else
        {
            Serial.println("[h360] no existe config previa, usando defaults.");
        }
    }

    void loop()
    {
        // lógica futura
    }

    H360Config &getConfig()
    {
        return config;
    }

    void applyConfig(const String &json)
    {
        Serial.println("[h360] applyConfig() recibido:");
        Serial.println(json);

        // Parsear JSON entrante
        H360Config newConfig;
        if (!newConfig.fromJson(json))
        {
            Serial.println("[h360] ❌ JSON inválido");
            return;
        }

        // Detectar cambios WiFi
        bool wifiChanged =
            (config.wifiSsid != newConfig.wifiSsid) ||
            (config.wifiPassword != newConfig.wifiPassword);

        // Detectar cambios MQTT
        bool mqttChanged =
            (config.mqttHost != newConfig.mqttHost) ||
            (config.mqttUser != newConfig.mqttUser) ||
            (config.mqttPass != newConfig.mqttPass) ||
            (config.mqttPort != newConfig.mqttPort);

        bool requiresReboot = wifiChanged || mqttChanged;

        // Guardar en NVS
        if (!storage.saveConfig(newConfig))
        {
            Serial.println("[h360] ❌ Error guardando en NVS");
            return;
        }

        Serial.println("[h360] ✔ Config guardada en NVS");

        // Actualizar config activa
        config = newConfig;

        // ========= ACK VÍA MQTT =========
        String ackTopic = "h360/device/" + config.deviceId + "/config/ack";

        StaticJsonDocument<128> doc;
        doc["status"] = "ok";
        doc["reboot"] = requiresReboot;

        char payload[128];
        serializeJson(doc, payload);

        mqtt.publish(ackTopic.c_str(), payload);

        Serial.print("[h360] ACK enviado: ");
        Serial.println(payload);

        // ========= REBOOT SI ES NECESARIO =========
        if (requiresReboot)
        {
            Serial.println("[h360] Reiniciando para aplicar nueva configuración...");
            delay(1000);
            ESP.restart();
        }
    }
}
