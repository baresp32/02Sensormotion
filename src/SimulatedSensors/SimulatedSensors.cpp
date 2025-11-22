#include "SimulatedSensors.h"


void SimulatedSensors::listen() {
    if (!Serial.available()) return;
    char c = Serial.read();
    if (c == '1') pirState = true;
    if (c == '0') pirState = false;
}

bool SimulatedSensors::readPir() {
    return pirState;
}
