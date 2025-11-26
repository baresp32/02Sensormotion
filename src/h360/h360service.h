#pragma once
#include <Arduino.h>
#include "h360config.h"
#include "h360storage.h"

namespace H360
{
    void setup();
    void loop();

    H360Config &getConfig();

    void applyConfig(const String &json);
}
