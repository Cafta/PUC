#include "FalconRobot.h"

#define sensorEsq A2
#define sensorDir A3

#define LIMIAR 850
#define SPEED 50
#define NUM_AMOSTRAS 5

#define LEFT  0
#define RIGHT 1

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
  else if (esq == 0 && dir == 0) {
    // Ambos fora da linha = parar totalmente
    executarMovimento(PARADO);
  }
}
void vel_motor(int8_t velocidade[2]){
  motors.leftDrive(velocidade[0], FORWARD);
  motors.rightDrive(velocidade[1], FORWARD);
}

void executarMovimento(Movimento mov) {
  int8_t velocidade[2] = {0,0};
  switch (mov) {
    case FRENTE:
      //motors.leftDrive(SPEED, FORWARD);
      //motors.rightDrive(SPEED, FORWARD);
      velocidade[LEFT] = SPEED;
      velocidade[RIGHT] = SPEED;
      break;
    case ESQUERDA:
      //motors.leftDrive(SPEED + 10, FORWARD);
      //motors.rightDrive(SPEED - 20, FORWARD);
      velocidade[LEFT] = SPEED + 10;
      velocidade[RIGHT] = SPEED - 20;
      break;
    case DIREITA:
      //motors.leftDrive(SPEED - 20, FORWARD);
      //motors.rightDrive(SPEED + 10, FORWARD);
      velocidade[LEFT] = SPEED - 20;
      velocidade[RIGHT] = SPEED + 10;
      break;
    case PARADO:
    default:
      //motors.leftDrive(0, FORWARD);
      //motors.rightDrive(0, FORWARD);
      velocidade[LEFT] = 0;
      velocidade[RIGHT] = 0;
      break;
      
  }
vel_motor(velocidade);
  // Atualiza último movimento
  ultimoMovimento = mov;
}
