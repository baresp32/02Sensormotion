#pragma once
#include <Arduino.h>

class PirSensor {
private:
    int pin;

public:
    PirSensor(int pirPin);
    bool read();
};
