#include "Core.h"
#include "../wifi/WifiManager.h"
#include "../bluetooth/BluetoothProvision.h"
#include "../mqtt/MqttService.h"
#include "../ntp/NtpService.h"
#include <ArduinoJson.h>

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

        Core::log("Iniciando sistema...");

        // 1) Cargar credenciales WiFi
        wifi.begin();

        // 2) Si no hay WiFi → provisioning
        if (!wifi.isConnected())
        {
            Core::log("WiFi no disponible, activar BT...");
            bt.begin();
        }
        else
        {
            Core::log("WiFi disponible.");
            Core::log(("IP: " + wifi.getIP()).c_str());
        }

        delay(500);
        NtpService::begin();
        delay(500);
        mqtt.begin();
        delay(500);
        mqtt.reconnect(); // << FORZAR CONEXIÓN AQUÍ
    }

    // ----------------------------------------------------
    // LOOP PRINCIPAL
    // ----------------------------------------------------
    void loop()
    {

        // Llamamos a la función para manejar el LED
        mqtt.reconnect();
        // 0) Offline total → Sleep seguro
        if (handleOfflineMode())
            return;

        // 1) Si NO hay WiFi → escuchar BT provisioning
        if (!wifi.isConnected())
        {
            bt.listen();
            return;
        }

        // 3) Log con timestamp NTP
        Serial.print("[");
        Serial.print(NtpService::now());
        Serial.println("]");

        static unsigned long last = 0;

        // 4) Cada 2 segundos → enviar MQTT
        if (millis() - last > 2000)
        {
            mqtt.loop();
            last = millis();

            StaticJsonDocument<128> doc;
            doc["timestamp"] = NtpService::now();

            char payload[128];
            serializeJson(doc, payload);

            mqtt.publish("esp32/test", payload);

            Serial.print("[MQTT] publicado: ");
            Serial.println(payload);
        }

        // 5) Log estado MQTT
        if (!mqtt.isConnected())
        {
            Serial.println("[MQTT] ❌ NO CONECTADO");
        }
        else
        {
            Serial.println("[MQTT] ✔ CONECTADO");
        }

        delay(5000);
    }

} // namespace Core
