#include <Arduino.h>

/*****************************************************************
 * Driver de 2 semáforos – protocolo binário de 6 bits
 *****************************************************************/
constexpr uint8_t P1_R = 5, P1_Y = 6, P1_G = 7;   // Semáforo 1
constexpr uint8_t P2_R = 2, P2_Y = 3, P2_G = 4;   // Semáforo 2
constexpr uint32_t BAUD = 115200;

// bits 5-3 = S1 (R,Y,G) | bits 2-0 = S2 (R,Y,G)
inline void updateLEDs(uint8_t frame)
{
  digitalWrite(P1_R, (frame & 0b100000) ? LOW : HIGH);
  digitalWrite(P1_Y, (frame & 0b010000) ? LOW : HIGH);
  digitalWrite(P1_G, (frame & 0b001000) ? LOW : HIGH);

  digitalWrite(P2_R, (frame & 0b000100) ? LOW : HIGH);
  digitalWrite(P2_Y, (frame & 0b000010) ? LOW : HIGH);
  digitalWrite(P2_G, (frame & 0b000001) ? LOW : HIGH);
}

void setup()
{
  pinMode(P1_R, OUTPUT); pinMode(P1_Y, OUTPUT); pinMode(P1_G, OUTPUT);
  pinMode(P2_R, OUTPUT); pinMode(P2_Y, OUTPUT); pinMode(P2_G, OUTPUT);

  // inicia com todos apagados (LED ligado a GND → HIGH = off)
  digitalWrite(P1_R, HIGH); digitalWrite(P1_Y, HIGH); digitalWrite(P1_G, HIGH);
  digitalWrite(P2_R, HIGH); digitalWrite(P2_Y, HIGH); digitalWrite(P2_G, HIGH);

  Serial.begin(BAUD);
}

void loop()
{
  if (Serial.available())
  {
    uint8_t frame = Serial.read();     // lê 1 byte bruto
    updateLEDs(frame);
    Serial.write('K');                 // opcional: ack de 1 byte
  }
}
