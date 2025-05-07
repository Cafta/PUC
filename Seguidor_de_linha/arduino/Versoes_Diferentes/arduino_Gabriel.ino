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
/*******************************************************************************
* Exp4_2_LineFollowing -- Falcon Robot Experiment 4.2
*
* This code reads the two line following sensors on A2 and A3
* and prints them out to the Serial Monitor. Upload this example to your
* Falcon Robot and open up the Serial Monitor by clicking the magnifying glass
* in the upper-right hand corner.
*
* This is a real simple example of a line following algorithm. It has
* a lot of room for improvement, but works fairly well for a curved track.
* It does not handle right angles reliably -- maybe you can come up with a
* better solution?
*
* Hardware setup:
* The Line Sensors must be connected to pins A2 and A3 (left and right sensor,
* respectively). The motors must be connected, and the board must be receiving
* power from the battery pack.
*
* This sketch was written by RoboCore, with lots of help from the Arduino
* community(especially from Sparkfun). This code is completely free for any use.
*
* Visit https://www.robocore.net/tutoriais/kit-iniciante-robotica-introducao
* for Falcon Robot Kit Information
*
* 20 Jul 2017 MarceloFariaz
*******************************************************************************/

#include "FalconRobot.h"
// initialize a sensor object on A2 and A3
FalconRobotLineSensor left(A2);
FalconRobotLineSensor right(A3);

int leftValue;  // variable to store the left sensor value
int rightValue;  // variable to store the right sensor value

// constants that are used in the code. LINETHRESHOLD is the level to detect
// if the sensor is on the line or not. If the sensor value is greater than this
// the sensor is above a DARK line.
//
// SPEED sets the nominal speed

#define LINETHRESHOLD 700
#define SPEED 50  // Set to any number from 0 - 100.

FalconRobotMotors motors(5, 7, 6, 8);
int leftSpeed;   // variable used to store the leftMotor speed
int rightSpeed;  // variable used to store the rightMotor speed


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

  Serial.println("Welcome to experiment 5.2 - Line Following");
  delay(2000);
  Serial.println("Line Sensor Readings: ");
  delay(500);
}

void loop() {
  // Read the sensors
  leftValue = left.read();
  rightValue = right.read();

  // Print the sensors values
  Serial.print(leftValue);
  Serial.print("\t");  // tab character
  Serial.print(rightValue);
  Serial.println();   // new line

  // if the both sensors are on the line, drive forward left and right at the same speed
  if((leftValue > LINETHRESHOLD) && (rightValue > LINETHRESHOLD)) {
  	leftSpeed = SPEED;
  	rightSpeed = SPEED;
  }

  // if the line only is under the right sensor, adjust relative speeds to turn to the right
  else if(rightValue > LINETHRESHOLD) {
  	leftSpeed = SPEED + 40;
  	rightSpeed = SPEED - 40;
  }

  // if the line only is under the left sensor, adjust relative speeds to turn to the left
  else if(leftValue > LINETHRESHOLD) {
  	leftSpeed = SPEED - 40;
  	rightSpeed = SPEED + 40;
  }

  // run motors given the control speeds above
  motors.leftDrive(leftSpeed, FORWARD);
  motors.rightDrive(rightSpeed, FORWARD);

  delay(0);  // add a delay to decrease sensitivity.
}
