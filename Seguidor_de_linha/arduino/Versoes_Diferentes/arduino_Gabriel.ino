#include "FalconRobot.h"

#define sensorEsq A2
#define sensorDir A3

#define LIMIAR 850
#define SPEED 50
#define NUM_AMOSTRAS 5

FalconRobotMotors motors(5, 7, 6, 8);

// Variável para lembrar o último movimento
enum Movimento { PARADO, FRENTE, ESQUERDA, DIREITA };
Movimento ultimoMovimento = FRENTE;

// Protótipos
void lerSensores(int &esq, int &dir);
void seguirLinha(int esq, int dir);
void executarMovimento(Movimento mov);
int mediaLeitura(int pino);

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor analógico com filtragem");
  delay(1000);
}

void loop() {
  int esq, dir;
  lerSensores(esq, dir);
  seguirLinha(esq, dir);
  delay(50);
}

void lerSensores(int &esq, int &dir) {
  int valorEsq = mediaLeitura(sensorEsq);
  int valorDir = mediaLeitura(sensorDir);

  esq = (valorEsq > LIMIAR) ? 1 : 0;
  dir = (valorDir > LIMIAR) ? 1 : 0;

  Serial.print("Sensor E: "); Serial.print(esq);
  Serial.print("  Sensor D: "); Serial.println(dir);
}

int mediaLeitura(int pino) {
  long soma = 0;
  for (int i = 0; i < NUM_AMOSTRAS; i++) {
    soma += analogRead(pino);
  }
  return soma / NUM_AMOSTRAS;
}

void seguirLinha(int esq, int dir) {
  if (esq == 1 && dir == 1) {
    executarMovimento(FRENTE);
  }
  else if (esq == 0 && dir == 1) {
    executarMovimento(ESQUERDA);
  }
  else if (esq == 1 && dir == 0) {
    executarMovimento(DIREITA);
  }
  else {
    // Ambos sensores fora da linha: continue com o último movimento por segurança
    executarMovimento(ultimoMovimento);
  }
}

void executarMovimento(Movimento mov) {
  switch (mov) {
    case FRENTE:
      motors.leftDrive(SPEED, FORWARD);
      motors.rightDrive(SPEED, FORWARD);
      break;
    case ESQUERDA:
      motors.leftDrive(SPEED + 10, FORWARD);
      motors.rightDrive(SPEED - 20, FORWARD);
      break;
    case DIREITA:
      motors.leftDrive(SPEED - 20, FORWARD);
      motors.rightDrive(SPEED + 10, FORWARD);
      break;
    case PARADO:
    default:
      motors.leftDrive(0, FORWARD);
      motors.rightDrive(0, FORWARD);
      break;
  }

  // Atualiza último movimento
  ultimoMovimento = mov;
}
