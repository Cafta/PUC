#include <Arduino.h>
#line 1 "D:\\github\\PUC\\Seguidor_de_linha\\arduino\\arduino.ino"
#line 1 "D:\\github\\PUC\\Seguidor_de_linha\\arduino\\arduino.ino"
void setup();
#line 5 "D:\\github\\PUC\\Seguidor_de_linha\\arduino\\arduino.ino"
void loop();
#line 1 "D:\\github\\PUC\\Seguidor_de_linha\\arduino\\arduino.ino"
void setup() {
    pinMode(13, OUTPUT);
  }
  
  void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(1000);
  }

