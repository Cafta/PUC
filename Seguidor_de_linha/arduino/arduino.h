/**
 * @file arduino.h
 * @brief This file contains the necessary includes and definitions for the Arduino project.
 * It includes the FalconRobot library and defines constants for sensor pins, speed, and thresholds.
 */

// Includes
#include "FalconRobot.h"
#include <time.h> // For delay function
#include <LiquidCrystal.h> // For LCD display

// Hardware definitions 
//    int sensorEsq = A2; // Left sensor pin
//    int sensorDir = A3; // Right sensor pin
#define BOTAO1_USUARIO A0 // Button pin
#define BOTAO2_USUARIO A1 // Button pin
LiquidCrystal lcd(12, 11, 13, A4, 10, 9);
FalconRobotLineSensor left(A3);
FalconRobotLineSensor right(A2);
FalconRobotMotors motors(5, 7, 6, 8);

// Software definitions
#define DEFAULT_SPEED 50 // Speed for the motors
#define DELAY_DE_RASTREAMENTO 100 // Delay for tracking (in ms)
// #define NUM_AMOSTRAS 5 // Não usado => Number of samples for averaging sensor readings

// Define constants
enum Movimento { ESQUERDA, DIREITA, TODOS, PARADO, FRENTE }; // atribui automaticamente 0, 1, 2, 3
enum Modos { NAVEGACAO, DECISAO }; // Modo de navegação
enum Comandos { ALARME, DESALARMAR, ERRO }; // Comandos para o bitDogLab
enum Estados { ESTACAO, ANDANDO }; // Estados de decisão

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
void bitDogLab_set(Comandos comando);
void andaLentamente(Movimento lado);
void decide();
void procurarLinha(Movimento lado);
void rodaCarrinho(Movimento lado, uint8_t decimalDaVelocidade); // roda o carrinho para um lado