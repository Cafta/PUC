/*******************************************************************************
* Arquivo principal da placa Arduino Julieta (RoboCore) do projeto de navegação do robô Falcon
*                      PUC Campinas - Maio 2025
*
*             Disciplina: Veículos Autônomos - 2025/1
*             Experimento - Navegação com Seguidor de Linha
*             Professor: Dr. EVERTON DIAS DE OLIVEIRA
*
*    Grupo 11:
*    Alunos: Carlos Amaral
*            Gabriel Morelli
*            Guilherme Talman
*            Murilo Trevisan
*******************************************************************************/
#include "arduino.h"  // Arquivo de cabeçalho do Projeto

// Variáveis Globais
uint8_t ultimaVelocidade[] = {0, 0}; // {esq, dir} | Última velocidade dos motores 
uint8_t ultimaLeitura[] = {0, 0}; // {esq, dir} | Última leitura dos sensores
Modos modo = NAVEGACAO; // Existe NAVEGACAO e DECISAO
uint8_t naLinha[] = {0, 0}; // {esq, dir} | 0 = fora da linha, 1 = na linha
uint8_t speed[] = {0, 0}; // {esq, dir} | velocidade dos motores
uint8_t flag_parado[] = {0, 0}; // Flag para indicar se o motor está parado
uint8_t flag_alarme = 0; // Flag para indicar se o alarme está ativado
int leftValue;  // variable to store the left sensor value
int rightValue;  // variable to store the right sensor value

void setup() {
  Serial.begin(9600);
  pinMode(BOTAO1_USUARIO, INPUT_PULLUP); // Ativa resistor pull-up interno

  Serial.println("Sensor analógico com filtragem");
  delay(1000);

  // CALIBRAÇÃO DOS SENSORES
    
    // Passo 1: Coloque os sensores fora da linha
    Serial.println("Coloque os sensores fora da linha e pressione o botão...");
    while (!botaoPressionado(BOTAO1_USUARIO));  // Espera usuário confirmar
    left.setBGLevel();
    right.setBGLevel();
  
    // Passo 2: Coloque os sensores sobre a linha
    Serial.println("Agora posicione os sensores SOBRE a linha e pressione o botão...");
    while (!botaoPressionado(BOTAO1_USUARIO));  // Espera de novo
    left.setDetectLevel();
    right.setDetectLevel();
  
    Serial.println("Calibração concluída!");
}

void loop() {
  if (modo == NAVEGACAO) {
    navegar();
  } else { // modo == DECISAO
    decide();
  }
  delay(50);
}

void decide() {

}

void lerSensores() {

// Atualiza a ultima leitura dos sensores a não ser que seja {0, 0} ou {1, 1}
// utilizado em decide(); para que lado procurar a linha?;
if (ultimaLeitura[ESQUERDA] != 0 || ultimaLeitura[DIREITA] != 0) {
  ultimaLeitura[ESQUERDA] = naLinha[ESQUERDA];
  ultimaLeitura[DIREITA] = naLinha[DIREITA];   
}

  // Lê os sensores (atualiza os valores de naLinha[])
  naLinha[ESQUERDA] = left.read();
  naLinha[DIREITA] = right.read();

  // Atualiza a ultima leitura dos sensores
  // utilizado em decide(); para que lado procurar a linha?;


  // Serial.print("Sensor E: "); 
  // Serial.print(left.check());
  // Serial.print("  Sensor D: "); 
  // Serial.println(right.check());
}



// /**
//  * @brief Não utilizado, seria para diminuir o ruído do sensor
//  * @param pino Pino do sensor a ser lido
//  */
// int mediaLeitura(int pino) {
//   long soma = 0;
//   for (int i = 0; i < NUM_AMOSTRAS; i++) {
//     soma += analogRead(pino);
//   }
//   return soma / NUM_AMOSTRAS;
// }


void navegar() {
  lerSensores();

  if (naLinha[ESQUERDA] == 0 && naLinha[DIREITA] == 0) {
    speed[ESQUERDA] = 0; // Ambos fora da linha, parar motores 
    speed[DIREITA] = 0; 
    modo = DECISAO; // mudar para modo de decisão
  }
  else if (naLinha[ESQUERDA] == 1 && naLinha[DIREITA] == 1) {
    // Ambos na linha = andar em frente
    speed[ESQUERDA] = DEFAULT_SPEED;
    speed[DIREITA] = DEFAULT_SPEED;
  }
  else if (naLinha[ESQUERDA] == 1 && naLinha[DIREITA] == 0) {
    // Sensor esquerdo na linha, direito fora = curva para a esquerda
    speed[ESQUERDA] = DEFAULT_SPEED * (1 + 0.4); // Aumenta velocidade do motor esquerdo
    speed[DIREITA] = DEFAULT_SPEED * (1 - 0.4); // Reduz velocidade do motor direito
  }
  else if (naLinha[ESQUERDA] == 0 && naLinha[DIREITA] == 1) {
    // Sensor direito na linha, esquerdo fora = curva para a direita
    speed[ESQUERDA] = DEFAULT_SPEED * (1 - 0.4); // Reduz velocidade do motor esquerdo
    speed[DIREITA] = DEFAULT_SPEED * (1 + 0.4); // Aumenta velocidade do motor direito
  }

  vel_motor();
}

uint8_t objetoNaVia() {
  // MUDAR
  return 0; 
}

void vel_motor() {
  if (objetoNaVia()) {
    // Se o objeto estiver na frente, pare os motores
    speed[ESQUERDA] = 0;
    speed[DIREITA] = 0;
    bitDogLab_set(ALARME); // Ativa alarme
  } 

  for (uint8_t i = 0; i < 2; i++) {
    if (ultimaVelocidade[i] == 0) {
      if (speed[i] < 30 && speed[i] != 0) {
        power_speed(i, speed[i]); // Liga o motor a 50% de potência para sair da inercia
      }
    }
    setMotor(i, speed[i]); // Liga o motor
    ultimaVelocidade[i] = speed[i]; // Atualiza último movimento
  }
}

int8_t botaoPressionado(int8_t botao) {
  // Verifica se o botão foi pressionado
  if (digitalRead(botao) == LOW) {
    delay(50); // Debounce delay
    if (digitalRead(botao) == LOW) {
      return 1; // Botão pressionado
    }
  }
  return 0; // Botão não pressionado
}

void bitDogLab_set(comandos comando) {
  // Envia comando para o bitDogLab
  if (comando == ALARME && flag_alarme == 0) {
    flag_alarme = 1; // Ativa alarme
    Serial.println("ALARME ATIVADO!");
  } else if (comando == DESALARMAR && flag_alarme == 1) {
    flag_alarme = 0; // Desativa alarme
    Serial.println("ALARME DESATIVADO!");
  }
}

void power_speed(uint8_t roda, uint8_t velocidade){
  setMotor(roda, 50); // Liga o motor a 50% de potência para sair da inercia
  delay(10); // O suficiente para sair da inercia 
  setMotor(roda, velocidade); // Volta a velocidade solicitada
}

void setMotor(int motor, int speed) {
  if (motor == 0) {
    motors.leftDrive(speed, FORWARD); // Liga o motor esquerdo
  } else if (motor == 1) {
    motors.rightDrive(speed, FORWARD); // Liga o motor direito
  } else {
    motors.leftDrive(speed, FORWARD);
    motors.rightDrive(speed, FORWARD);
  }
}