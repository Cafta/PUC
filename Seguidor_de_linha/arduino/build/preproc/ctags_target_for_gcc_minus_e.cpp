# 1 "D:\\github\\PUC\\Seguidor_de_linha\\arduino\\arduino.ino"
void setup() {
    pinMode(13, 0x1);
  }

  void loop() {
    digitalWrite(13, 0x1);
    delay(100);
    digitalWrite(13, 0x0);
    delay(1000);
  }
