#include "NtpService.h"
#include <WiFi.h>
#include <time.h>

void NtpService::begin()
{
    configTime(-3 * 3600, 0,
               "time.google.com",
               "pool.ntp.org",
               "time.nist.gov");

    Serial.println("[NTP] Sincronizando...");

    struct tm timeinfo;
    int retries = 0;

    while (!getLocalTime(&timeinfo) && retries < 20)
    {
        Serial.print(".");
        retries++;
        delay(500);
    }

    if (retries >= 20)
    {
        Serial.println("\n[NTP] ❌ No se pudo obtener hora.");
    }
    else
    {
        Serial.println("\n[NTP] ✔ Hora sincronizada.");
    }
}

String NtpService::now() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "NTP_ERROR";
    }

    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}
