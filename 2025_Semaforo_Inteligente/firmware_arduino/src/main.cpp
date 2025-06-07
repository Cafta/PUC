#include <Arduino.h>

void setup() {
  Serial.begin(19200);
}

void loop() {
  // vazio
  Serial.println("Hello");
  delay(500);
}