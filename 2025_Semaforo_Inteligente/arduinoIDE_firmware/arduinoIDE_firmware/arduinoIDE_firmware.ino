constexpr uint8_t P1_R = 3, P1_Y = 4, P1_G = 5;   // Semáforo 1
constexpr uint8_t P2_R = 6, P2_Y = 7, P2_G = 8;   // Semáforo 2
constexpr uint32_t BAUD = 9600;

void setup() {
  pinMode(P1_R, OUTPUT); pinMode(P1_Y, OUTPUT); pinMode(P1_G, OUTPUT);
  pinMode(P2_R, OUTPUT); pinMode(P2_Y, OUTPUT); pinMode(P2_G, OUTPUT);

  // Apaga todos
  digitalWrite(P1_R, HIGH); digitalWrite(P1_Y, HIGH); digitalWrite(P1_G, HIGH);
  digitalWrite(P2_R, HIGH); digitalWrite(P2_Y, HIGH); digitalWrite(P2_G, HIGH);

  Serial.begin(BAUD);
  Serial.println("Digite um número de 0 a 63 e pressione Enter:");
}

void updateLEDs(uint8_t frame) {
  Serial.print("Atualizando LEDs com valor: ");
  Serial.println(frame);

  digitalWrite(P1_R, (frame & 0b100000) ? LOW : HIGH);
  digitalWrite(P1_Y, (frame & 0b010000) ? LOW : HIGH);
  digitalWrite(P1_G, (frame & 0b001000) ? LOW : HIGH);

  digitalWrite(P2_R, (frame & 0b000100) ? LOW : HIGH);
  digitalWrite(P2_Y, (frame & 0b000010) ? LOW : HIGH);
  digitalWrite(P2_G, (frame & 0b000001) ? LOW : HIGH);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // remove espaços e quebras

    if (input.length() == 0) return;

    int frame = input.toInt();
    if (frame >= 0 && frame <= 63) {
      updateLEDs((uint8_t)frame);
    } else {
      Serial.println("Valor inválido! Use de 0 a 63.");
    }
  }
}

