#include "BluetoothProvision.h"


BluetoothProvision::BluetoothProvision(WifiManager *ref)
{
    wifi = ref;
}

void BluetoothProvision::begin()
{
    BT.begin("ESP32-Setup");
    Serial.println("[BT] Bluetooth listo. Formato: ssid;password");
}

void BluetoothProvision::listen()
{
    if (!BT.available())
        return;

    // Cortar mensaje al recibir ENTER
    String data = BT.readStringUntil('\n');
    data.trim();

    Serial.print("[BT] Recibido: ");
    Serial.println(data);

    int sep = data.indexOf(';');
    if (sep < 1)
    {
        Serial.println("[BT] Formato inválido. Use ssid;pass");
        return;
    }

    String ssid = data.substring(0, sep);
    String pass = data.substring(sep + 1);

    ssid.trim();
    pass.trim();

    Serial.println("[BT] Guardando credenciales...");
    wifi->save(ssid, pass);

    Serial.println("[BT] Reiniciando WiFi...");
    WiFi.disconnect();
    delay(300);

    Serial.print("[BT] Intentando conectar a: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        Serial.print(".");
        delay(400);
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[BT] ✔️ Conexión exitosa!");
        Serial.print("[BT] IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("[BT] Conectado a SSID: ");
        Serial.println(ssid);
    }
    else {
        Serial.println("[BT] ❌ No se logró conectar. Verifique SSID o password.");
    }
}