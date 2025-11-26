#include <ArduinoJson.h>
#include "Core.h"
#include "../h360/h360service.h"
#include "../wifi/WifiManager.h"
#include "../bluetooth/BluetoothProvision.h"
#include "../ntp/NtpService.h"
#include "../mqtt/MqttConfig.h"
#include "../mqtt/MqttService.h"

#include <BluetoothSerial.h>

// --- INSTANCIAS GLOBALES ---
BluetoothSerial SerialBT;
WifiManager wifi;
BluetoothProvision bt(&wifi);
MqttConfig mqttcfg;
MqttService mqtt;

static unsigned long lastWifiAttempt = 0;
static unsigned long backoff = 10; // Sleep inteligente
#define LED_PIN 2
bool setupMode = false;

namespace Core
{
    // ----------------------------------------------------
    // LOG
    // ----------------------------------------------------
    void log(const char *msg)
    {
        Serial.print("[CORE] ");
        Serial.println(msg);
    }

    // ----------------------------------------------------
    // WIFI + BLUETOOTH
    // ----------------------------------------------------
    void handleWifi()
    {
        if (!wifi.isConnected())
        {
            // Bluetooth ON mientras no haya WiFi
            bt.listen();

            // Reintentar conexión cada 30s (no spam)
            if (millis() - lastWifiAttempt > 30000)
            {
                Serial.println("[WiFi] Reintentando conexión...");
                wifi.reconnect();
                lastWifiAttempt = millis();
            }

            return;
        }

        // Si ya hay WiFi → apagar BT
        bt.stop();
    }

    // ----------------------------------------------------
    // MQTT
    // ----------------------------------------------------
    void handleMqtt()
    {
        if (!wifi.isConnected())
            return;

        if (!mqtt.isConnected())
        {
            Serial.println("[MQTT] Reintentando conexión...");
            mqtt.reconnect();
            return;
        }

        mqtt.loop(); // obligatorio
    }

    // ----------------------------------------------------
    // SLEEP inteligente (opcional)
    // ----------------------------------------------------
    bool handleOfflineMode()
    {
        if (!wifi.isConnected())
        {
            Serial.println("[OFFLINE] No WiFi → sleep " + String(backoff) + "s");

            esp_sleep_enable_timer_wakeup(backoff * 1000000ULL);
            esp_light_sleep_start();

            backoff = min(backoff * 2, (unsigned long)300);
            return true;
        }

        backoff = 10; // Reset
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

        // Config interna H360
        H360::setup();
        auto &cfg = H360::getConfig();

        // --- WIFI ---
        if (cfg.wifiSsid.length() == 0)
        {
            Serial.println("[CORE] Sin WiFi H360 → uso WifiManager (NVS)");
            wifi.begin();

            if (!wifi.isConnected())
            {
                Serial.println("[CORE] No hay WiFi almacenado → iniciar Bluetooth");
                bt.begin();
                return; // esperar credenciales por BT
            }
        }
        else
        {
            wifi.begin(cfg.wifiSsid, cfg.wifiPassword);
        }

        Serial.println("[CORE] WiFi OK, continuando...");

        // --- MQTT ---
        mqttcfg.load();
        Serial.println("=== DEBUG MQTT NVS ===");
        Serial.println("Host: [" + mqttcfg.host + "]");
        Serial.println("Port: " + String(mqttcfg.port));
        Serial.println("User: [" + mqttcfg.user + "]");
        Serial.println("Pass: [" + mqttcfg.pass + "]");
        Serial.println("======================");
        if (mqttcfg.host.length() == 0 || mqttcfg.port == 0)
        {
            Serial.println("[CORE] ⚠ MQTT vacío → usando bootstrap (secrets.h)");
            mqtt.begin(MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
            setupMode = true;   // 👈 AGREGAR SOLO ESTO            
        }
        else
        {
            mqtt.begin(mqttcfg.host, mqttcfg.port, mqttcfg.user, mqttcfg.pass);
        }

        // --- NTP ---
        NtpService::begin();
    }

    // ----------------------------------------------------
    // LOOP
    // ----------------------------------------------------
    void loop()
    {
        handleWifi();
        if (!wifi.isConnected()) return;

        if (setupMode) {     // 👈 SOLO escuchar la config
            mqtt.loop();
            return;
        }

        handleMqtt();        // 👈 modo normal

    }

} // namespace Core
