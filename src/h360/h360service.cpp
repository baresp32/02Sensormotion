#include "h360service.h"
#include "../mqtt/MqttConfig.h"

#include <ArduinoJson.h>

static H360Config config;
static H360Storage storage;

extern MqttConfig mqttcfg; // instancia global

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

    // ⬇⬇⬇ CORREGIDO AQUÍ ⬇⬇⬇
    void applyConfig(const String &json)
    {
        StaticJsonDocument<512> doc;
        deserializeJson(doc, json);

        mqttcfg.host = doc["network"]["mqtt"]["host"] | mqttcfg.host;
        mqttcfg.port = doc["network"]["mqtt"]["port"] | mqttcfg.port;
        mqttcfg.user = doc["network"]["mqtt"]["username"] | mqttcfg.user;
        mqttcfg.pass = doc["network"]["mqtt"]["password"] | mqttcfg.pass;

        mqttcfg.save();
        delay(300);
        ESP.restart();
    }
}
