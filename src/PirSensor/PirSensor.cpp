#include "PirSensor.h"



PirSensor::PirSensor(int pirPin) {
    pin = pirPin;
    pinMode(pin, INPUT);
}

bool PirSensor::read() {
    return digitalRead(pin);
}
