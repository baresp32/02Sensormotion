#ifndef CORE_H
#define CORE_H

#include <Arduino.h> // ← NECESARIO para usar String

namespace Core
{
    void setup();
    void loop();

    

    void log(const String &msg);
    void log(const char *msg);
}

#endif // CORE_H
