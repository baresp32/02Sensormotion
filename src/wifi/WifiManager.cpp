#include "WifiManager.h"

void WifiManager::begin()
{
    Serial.println("[WiFi] === Iniciando WifiManager ===");

    load(); // Cargar desde NVS

    if (ssid.length() == 0)
    {
        Serial.println("[WiFi] No había credenciales en NVS. Usando secrets.h");

        ssid = DEFAULT_SSID;
        pass = DEFAULT_PASSWORD;
    }
    else
    {
        Serial.println("[WiFi] Credenciales cargadas desde NVS");
    }

    Serial.print("[WiFi] SSID a conectar: ");
    Serial.println(ssid);

    Serial.print("[WiFi] PASS a conectar: ");
    Serial.println(pass);

    Serial.println("[WiFi] Llamando WiFi.begin...");
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[WiFi] 👍 Conectado!");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("[WiFi] SSID conectado: ");
        Serial.println(WiFi.SSID());
    }
    else
    {
        Serial.println("[WiFi] ❌ No se pudo conectar.");
    }
}

void WifiManager::begin(const String &s, const String &p)
{
    Serial.println("[WiFi] === Iniciando WifiManager (con parámetros) ===");

    ssid = s;
    pass = p;

    Serial.print("[WiFi] SSID recibido: ");
    Serial.println(ssid);

    Serial.print("[WiFi] PASS recibido: ");
    Serial.println(pass);

    Serial.println("[WiFi] Llamando WiFi.begin...");
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[WiFi] 👍 Conectado!");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("[WiFi] SSID conectado: ");
        Serial.println(WiFi.SSID());
    }
    else
    {
        Serial.println("[WiFi] ❌ No se pudo conectar.");
    }
}

void WifiManager::load()
{
    prefs.begin("wifi", true);
    ssid = prefs.getString("ssid", "");
    pass = prefs.getString("pass", "");
    prefs.end();
}

void WifiManager::save(const String &s, const String &p)
{
    prefs.begin("wifi", false);
    prefs.putString("ssid", s);
    prefs.putString("pass", p);
    prefs.end();
}

bool WifiManager::hasCredentials()
{
    load();
    return ssid.length() > 0 && pass.length() > 0;
}

void WifiManager::setCredentials(const String &s, const String &p)
{
    ssid = s;
    pass = p;
}

bool WifiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WifiManager::getIP()
{
    return WiFi.localIP().toString();
}
