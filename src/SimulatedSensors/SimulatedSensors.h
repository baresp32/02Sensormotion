#pragma once
#include <Arduino.h>

class SimulatedSensors {
private:
    bool pirState = false;

public:
    void listen();
    bool readPir();
};
