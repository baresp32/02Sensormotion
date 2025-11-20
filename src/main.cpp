#include <Arduino.h>
//secretos para proyecto
#include "secrets.h"

const int LED_PIN = 23;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Iniciando...");
}

void loop()
{
  Serial.println("Parpadeando LED...");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(3000);
}
