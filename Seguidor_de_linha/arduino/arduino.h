/**
 * @file arduino.h
 * @brief This file contains the necessary includes and definitions for the Arduino project.
 * It includes the FalconRobot library and defines constants for sensor pins, speed, and thresholds.
 */

// Includes
#include "FalconRobot.h"

// Hardware definitions 
//    int sensorEsq = A2; // Left sensor pin
//    int sensorDir = A3; // Right sensor pin
#define BOTAO1_USUARIO A0 // Button pin
#define BOTAO2_USUARIO A1 // Button pin
FalconRobotLineSensor left(A2);
FalconRobotLineSensor right(A3);

FalconRobotMotors motors(5, 7, 6, 8);

// Software definitions
#define DEFAULT_SPEED 50 // Speed for the motors
// #define NUM_AMOSTRAS 5 // Não usado => Number of samples for averaging sensor readings

// Define constants
enum Movimento { ESQUERDA, DIREITA, TODOS, PARADO, FRENTE }; // atribui automaticamente 0, 1, 2, 3
enum Modos { NAVEGACAO, DECISAO }; // Modo de navegação
enum comandos { ALARME, DESALARMAR }; // Comandos para o bitDogLab

// Function prototypes
void lerSensores();
void navegar();
void vel_motor(); // verifica se o objeto está na frente e ajusta a velocidade dos motores
void setMotor(int motor, int speed); // não verifica nada, só muda a velocidade do motor
// void executarMovimento(Movimento mov);
uint8_t objetoNaVia();
int mediaLeitura(int pino);
int8_t botaoPressionado(int8_t botao);
void power_speed(int8_t roda, uint8_t velocidade);
void bitDogLab_set(comandos comando);