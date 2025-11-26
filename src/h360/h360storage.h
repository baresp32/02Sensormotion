#pragma once
#include <Arduino.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "h360config.h"

class H360Storage {
private:
    const char* NAMESPACE = "h360";
    const char* KEY = "shadow";

public:
    void begin();
    bool saveConfig(const H360Config& cfg);
    bool loadConfig(H360Config& cfg);
};
