#include "Core.h"
#include "../h360/h360service.h"
#include "../wifi/WifiManager.h"
#include "../bluetooth/BluetoothProvision.h"
#include "../mqtt/MqttService.h"
#include "../ntp/NtpService.h"
#include <ArduinoJson.h>

#include <BluetoothSerial.h> // Asegúrate de incluir la librería

// Definir SerialBT globalmente
BluetoothSerial SerialBT; // Aquí definimos el objeto global SerialBT

// Objetos globales
WifiManager wifi;
BluetoothProvision bt(&wifi);

MqttService mqtt;

#define LED_PIN 2 // El pin del LED de la placa (GPIO2)

namespace Core
{
    // ----------------------------------------------------
    // LOGS
    // ----------------------------------------------------
    void log(const String &msg)
    {
        Serial.print("[CORE] ");
        Serial.println(msg);
    }

    void log(const char *msg)
    {
        Serial.print("[CORE] ");
        Serial.println(msg);
    }

    // ----------------------------------------------------
    // OFFLINE MODE (sleep inteligente)
    // ----------------------------------------------------
    static unsigned long backoff = 10; // segundos

    bool handleOfflineMode()
    {
        if (!wifi.isConnected() || !mqtt.isConnected())
        {
            Core::log("[OFFLINE] No WiFi / No MQTT → sleep " + String(backoff) + "s");

            esp_sleep_enable_timer_wakeup(backoff * 1000000ULL);
            esp_light_sleep_start();

            // Exponencial: 10 → 20 → 40 → 80 → 160 → 300 máx
            backoff = min(backoff * 2, (unsigned long)300);
            return true;
        }

        // Si volvemos a estar online → resetear
        backoff = 10;
        return false;
    }

    // ----------------------------------------------------
    // SETUP
    // ----------------------------------------------------
    void setup()
    {

        Serial.begin(115200);
        delay(300);

        Serial.println("[CORE] Inicializando...");

        H360::setup();                 // cargar config desde NVS
        auto &cfg = H360::getConfig(); // obtener config cargada

        // ======== WIFI DINÁMICO ========
        wifi.begin(cfg.wifiSsid, cfg.wifiPassword);

        if (!wifi.isConnected())
        {
            Serial.println("[CORE] WiFi no disponible → iniciar BT provisioning");
            bt.begin();
            return; // ← IMPORTANTE
        }

        Serial.println("[CORE] WiFi conectado. Continuando setup...");

        // 3) MQTT SOLO SI HAY WIFI
        mqtt.begin(cfg.mqttHost, cfg.mqttPort, cfg.mqttUser, cfg.mqttPass);

        NtpService::begin();
        delay(1000);
    }

    // ----------------------------------------------------
    // LOOP PRINCIPAL
    // ----------------------------------------------------
    void loop()
    {
        // 1) Si hay WiFi → asegurar que BT esté apagado
        if (wifi.isConnected())
        {
            bt.stop();
        }

        // 1) Si NO hay WiFi → SOLO Bluetooth provisioning
        if (!wifi.isConnected())
        {
            bt.listen(); // permitir que el usuario envíe SSID/PASS
            return;      // < — MUY IMPORTANTE
        }

        // 2) Offline → Sleep inteligente (NO dormir si BT está activo)

        if (!wifi.isConnected())
        {
            Serial.println("[WiFi] No conectado a WiFi.");
        }
        if (!mqtt.isConnected())
        {
            Serial.println("[MQTT] No conectado al broker MQTT.");
            mqtt.reconnect();
        }

        if (!wifi.isConnected() || !mqtt.isConnected())
        {
            if (!bt.isActive())
            {
                if (handleOfflineMode())
                    return;
            }
        }

        // 3) Si no hay WiFi → escuchar provisioning Bluetooth
        if (!wifi.isConnected())
        {
            bt.listen();
            return;
        }

        // 4) MQTT loop SIEMPRE (required)
        mqtt.loop();

        // 5) Log con timestamp NTP (tú decides si dejarlo)
        Serial.print("[");
        Serial.print(NtpService::now());
        Serial.println("]");

        // 6) Cada 2 segundos → enviar payload JSON
        static unsigned long lastPublish = 0;

        if (millis() - lastPublish >= 2000)
        {

            StaticJsonDocument<128> doc;
            doc["timestamp"] = NtpService::now();

            char payload[128];
            serializeJson(doc, payload);

            mqtt.publish("esp32/test", payload);

            Serial.print("[MQTT] Publicado: ");
            Serial.println(payload);

            lastPublish = millis();
        }

        // 8) Pequeño delay suave (no bloquear)
        delay(10); // 10 ms es suficiente y seguro
    }

} // namespace Core
