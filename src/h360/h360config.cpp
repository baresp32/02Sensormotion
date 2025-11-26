#include "h360config.h"

String H360Config::toJson() const
{
    DynamicJsonDocument doc(2048);

    doc["version"] = version;

    doc["device"]["id"] = deviceId;
    doc["device"]["location"] = location;
    doc["device"]["name"] = name;

    doc["network"]["wifi"]["ssid"] = wifiSsid;
    doc["network"]["wifi"]["password"] = wifiPassword;
    doc["network"]["wifi"]["fallbackSsid"] = wifiFallbackSsid;
    doc["network"]["wifi"]["fallbackPassword"] = wifiFallbackPassword;

    doc["network"]["mqtt"]["host"] = mqttHost;
    doc["network"]["mqtt"]["port"] = mqttPort;
    doc["network"]["mqtt"]["username"] = mqttUser;
    doc["network"]["mqtt"]["password"] = mqttPass;

    String out;
    serializeJson(doc, out);
    return out;
}

bool H360Config::fromJson(const String &json)
{
    DynamicJsonDocument doc(2048);
    auto err = deserializeJson(doc, json);
    if (err)
        return false;

    if (doc["version"].is<uint32_t>())
        version = doc["version"];

    if (doc["device"]["id"].is<const char *>())
        deviceId = doc["device"]["id"].as<const char *>();

    if (doc["device"]["location"].is<const char *>())
        location = doc["device"]["location"].as<const char *>();

    if (doc["device"]["name"].is<const char *>())
        name = doc["device"]["name"].as<const char *>();

    if (doc["network"]["wifi"]["ssid"].is<const char *>())
        wifiSsid = doc["network"]["wifi"]["ssid"].as<const char *>();

    if (doc["network"]["wifi"]["password"].is<const char *>())
        wifiPassword = doc["network"]["wifi"]["password"].as<const char *>();

    if (doc["network"]["wifi"]["fallbackSsid"].is<const char *>())
        wifiFallbackSsid = doc["network"]["wifi"]["fallbackSsid"].as<const char *>();

    if (doc["network"]["wifi"]["fallbackPassword"].is<const char *>())
        wifiFallbackPassword = doc["network"]["wifi"]["fallbackPassword"].as<const char *>();

    if (doc["network"]["mqtt"]["host"].is<const char *>())
        mqttHost = doc["network"]["mqtt"]["host"].as<const char *>();

    if (doc["network"]["mqtt"]["port"].is<int>())
        mqttPort = doc["network"]["mqtt"]["port"].as<int>();

    if (doc["network"]["mqtt"]["username"].is<const char *>())
        mqttUser = doc["network"]["mqtt"]["username"].as<const char *>();

    if (doc["network"]["mqtt"]["password"].is<const char *>())
        mqttPass = doc["network"]["mqtt"]["password"].as<const char *>();

    return true;
}
