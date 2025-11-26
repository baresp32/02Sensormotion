#include "Core.h"
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
            // 🔥 APAGAR BLUETOOTH SI YA TENEMOS WIFI
            bt.stop();
            Core::log("Bluetooth apagado (ya tenemos WiFi).");
            NtpService::begin();
            mqtt.begin();
            // Agregar un pequeño delay aquí para asegurarnos de que la conexión a MQTT esté lista.
            delay(1000); // Espera de 1 segundo, ajustable según sea necesario.

            // Ahora puedes intentar reconectar el cliente MQTT
            if (!mqtt.isConnected())
            {
                Serial.println("[MQTT] Intentando reconectar...");
                mqtt.reconnect();
            }
        }
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
