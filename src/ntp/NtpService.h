#pragma once
#include <Arduino.h>

class NtpService {
public:
    static void begin();
    static String now();
};
