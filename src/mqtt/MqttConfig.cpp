#include "MqttConfig.h"

#define NAMESPACE "mqttcfg"

void MqttConfig::load()
{
    prefs.begin(NAMESPACE, false);  // read/write (RECOMENDADO)

    

    host = prefs.getString("host", "");
    port = prefs.getInt("port", 0);
    user = prefs.getString("user", "");
    pass = prefs.getString("pass", "");

    prefs.end();

    Serial.println("[MQTT_CFG] Cargado desde NVS:");
    Serial.println("  host: " + host);
    Serial.println("  port: " + String(port));
    Serial.println("  user: " + user);
}

void MqttConfig::save()
{
    prefs.begin(NAMESPACE, false);

    prefs.putString("host", host);
    prefs.putInt("port", port);
    prefs.putString("user", user);
    prefs.putString("pass", pass);

    prefs.end();

    Serial.println("[MQTT_CFG] Guardado en NVS");
}
