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
Estados estadoDeDecisao = ANDANDO; // Estados de decisão
uint8_t naLinha[] = {0, 0}; // {esq, dir} | 0 = fora da linha, 1 = na linha
uint8_t speed[] = {0, 0}; // {esq, dir} | velocidade dos motores
uint8_t flag_parado[] = {0, 0}; // Flag para indicar se o motor está parado
uint8_t flag_alarme = 0; // Flag para indicar se o alarme está ativado
int leftValue;  // variable to store the left sensor value
int rightValue;  // variable to store the right sensor value

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(BOTAO1_USUARIO, INPUT_PULLUP); // Ativa resistor pull-up interno
  pinMode(BOTAO2_USUARIO, INPUT_PULLUP); // Ativa resistor pull-up interno

  Serial.println("Sensor analógico com filtragem");
  delay(1000);

  // CALIBRAÇÃO DOS SENSORES
    
    // Passo 1: Coloque os sensores fora da linha
    Serial.println("Coloque os sensores fora da linha e pressione o botão...");
    lcd.setCursor(0, 0);
    lcd.print(" Calibrando...  ");
    lcd.setCursor(0, 1);
    lcd.print("Fora linha + B0 ");
    while (!botaoPressionado(BOTAO1_USUARIO));  // Espera usuário confirmar
    left.setBGLevel();
    right.setBGLevel();
  
    // Passo 2: Coloque os sensores sobre a linha
    Serial.println("Agora posicione os sensores SOBRE a linha e pressione o botão...");
    lcd.setCursor(0, 1);
    lcd.print("Na linha + B0   ");
    while (!botaoPressionado(BOTAO1_USUARIO));  // Espera de novo
    left.setDetectLevel();
    right.setDetectLevel();
  
    Serial.println("Calibração concluída!");
    lcd.setCursor(0, 0);
    lcd.print("   Calibrado!   ");
    lcd.setCursor(0, 1);
    lcd.print("   ANDANDO...   ");
}

void loop() {
  if (modo == NAVEGACAO) {
    navegar();
  } else { // modo == DECISAO
    decide();
    Serial.println("Entrou modo Decisao. Volte para o moda navegação apertando Btn0");
    while (!botaoPressionado(BOTAO1_USUARIO));  // Espera usuário confirmar
    modo = NAVEGACAO; // Retorna para o modo de navegação
  }
  delay(1);
}

void decide() {
  Serial.println("Modo de decisão: ");
  display("Modo de decisao", "verificando cod");
  setMotor(TODOS, 0); // Desliga os motores
  
  if (estadoDeDecisao == ESTACAO) {
    display("Modo de decisao", "ESTACAO...     ");
    while (!botaoPressionado(BOTAO1_USUARIO));
    return; // Não faz nada, espera decisão do BitDogLab
  } else if (estadoDeDecisao == ANDANDO) {
    andaLentamente(); 
    lerSensores(); 
    if (naLinha[ESQUERDA] == 1 && naLinha[DIREITA] == 1) {
      display("Modo de decisao", "ESTACAO...     ");
      while (!botaoPressionado(BOTAO1_USUARIO));
      // estadoDeDecisao = ESTACAO; 
    } else { //if (naLinha[ESQUERDA] == 0 && naLinha[DIREITA] == 0) {
      display("Modo de decisao", "PROCURA...     ");
      // while (!botaoPressionado(BOTAO1_USUARIO));
      // Recupera última leitura dos sensores
      if (ultimaLeitura[ESQUERDA] == 1 && ultimaLeitura[DIREITA] == 0) {
              display("Modo de decisao", "PROCURA... ESQ ");
        procurarLinha(ESQUERDA); // Procura linha à esquerda
      } else {
        display("Modo de decisao", "PROCURA... DIR ");
        procurarLinha(DIREITA); // Procura linha à direita
      }
      // estadoDeDecisao = ANDANDO; // Retorna para o estado de decisão
      // modo = NAVEGACAO; // Retorna para o modo de navegação
    }
  }
}

void procurarLinha(Movimento lado) {
  // Lê os sensores (atualiza os valores de naLinha[])
  uint8_t achou = 0;
  uint8_t n_de_procuras = 0;

  // DEFINE O MOVIMENTO DOS MOTORES DEPENDENDO DO LADO
  rodaCarrinho(lado, 0.4);
  vel_motor(); // Atualiza a velocidade dos motores
  uint32_t start = millis(); // Marca o tempo de início
  uint32_t tempo_de_procura = 3000; // Tempo de procura em milissegundos

  while (achou == 0) {
    lerSensores(); // Lê os sensores
    if (start + tempo_de_procura < millis()) {
      // Se o tempo de procura para este lado acabou
      lado = (lado == ESQUERDA) ? DIREITA : ESQUERDA; // Muda o lado
      rodaCarrinho(lado, 0.4); // Liga os motores para o outro lado
      start = millis(); // Reinicia o tempo
      tempo_de_procura *= 2; // agora o tempo de procura pro outro lado é o dobro para voltar o que já foi
      vel_motor(); // Atualiza a velocidade dos motores
      n_de_procuras++;
    }
    if (n_de_procuras >= 2) {
      setMotor(TODOS, 0); // Desliga os motores
      bitDogLab_set(ERRO); // envia comando de erro para o bitDogLab
      break;
    }
    // Se chegou até aqui é porque ainda está procurando a linha
    if (naLinha[ESQUERDA] == 1 || naLinha[DIREITA] == 1) {
      // Encontrou a linha
      achou = 1; // Marca que encontrou a linha
    }
  }
}

void lerSensores() {
  // Atualiza a ultima leitura dos sensores a não ser que seja {0, 0} ou {1, 1}
  // utilizado em decide(); para que lado procurar a linha?;
  if (ultimaLeitura[ESQUERDA] != 0 || ultimaLeitura[DIREITA] != 0) {
    ultimaLeitura[ESQUERDA] = naLinha[ESQUERDA];
    ultimaLeitura[DIREITA] = naLinha[DIREITA];   
  }

  // Lê os sensores (atualiza os valores de naLinha[])
  naLinha[ESQUERDA] = left.check();
  naLinha[DIREITA] = right.check();

  // Serial.print("Sensor E: "); 
  // Serial.print(left.check());
  // Serial.print("  Sensor D: "); 
  // Serial.println(right.check());
}

void andaLentamente() {
  setMotor(TODOS, 0); // Desliga os motores
  delay(500); 
  power_speed(TODOS, 25); // Liga o motor esquerdo com velocidade 10
  // Liga os motores com velocidade 10
  // setMotor(TODOS, 10); // Liga o motor
  delay(DELAY_DE_RASTREAMENTO); 
  // Desliga os motores
  setMotor(TODOS, 0); // Desliga os motores
  // while (!botaoPressionado(BOTAO1_USUARIO)); // Espera usuário confirmar
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

  Serial.print("  Sensor E: ");
  Serial.print(naLinha[ESQUERDA]); 
  Serial.print("  Sensor D: ");
  Serial.println(naLinha[DIREITA]);

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
  } else if (flag_alarme) {
    // Se o alarme estiver ativado e não houver objeto na frente, desative o alarme
    bitDogLab_set(DESALARMAR); // Desativa alarme
    flag_alarme = 0; // Desativa alarme
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
    delay(100); // Debounce delay
    if (digitalRead(botao) == LOW) {
      return 1; // Botão pressionado
    }
  }
  return 0; // Botão não pressionado
}

void bitDogLab_set(Comandos comando) {
  // Envia comando para o bitDogLab
  // INTEGRAÇÃO COM MURILO
  if (comando == ALARME && flag_alarme == 0) {
    flag_alarme = 1; // Ativa alarme
    Serial.println("ALARME ATIVADO!");
  } else if (comando == DESALARMAR && flag_alarme == 1) {
    flag_alarme = 0; // Desativa alarme
    Serial.println("ALARME DESATIVADO!");
  } else if (comando == ERRO) {
    Serial.println("ERRO: Não consegui encontrar a linha!");
    modo = DECISAO; // Muda para modo de decisão
    estadoDeDecisao = ESTACAO; // Neste estado para e fica aguardando o bitDogLab
  } else {
    Serial.println("Comando inválido!");
  }
}

/**
 * @brief Power the motor to a specific speed CORRECTING
 * @param roda Motor to power (0 = left, 1 = right)
 * @param velocidade Speed to set (0-100)
 */
void power_speed(uint8_t roda, uint8_t velocidade){
  if(roda == 2) {
    setMotor(0, 50); // Liga o motor a 50% de potência para sair da inercia
    setMotor(1, 50); // Liga o motor a 50% de potência para sair da inercia
    delay(15); // O suficiente para sair da inercia 
    setMotor(0, velocidade); // Volta a velocidade solicitada
    setMotor(1, velocidade); // Volta a velocidade solicitada
  } else {
    setMotor(roda, 50); // Liga o motor a 50% de potência para sair da inercia
    delay(15); // O suficiente para sair da inercia 
    setMotor(roda, velocidade); // Volta a velocidade solicitada
  }
}

/**
 * @brief Set the motor speed
 * @param motor Motor to set (0 = left, 1 = right, 2 = both)
 * @param speed Speed to set (0-100)
 */
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

void rodaCarrinho(Movimento lado, uint8_t decimalDaVelocidade) {
  // Liga os motores para o lado especificado
  if (lado == ESQUERDA) {
    motors.leftDrive(20, FORWARD); // esquerdo pra frente
    motors.rightDrive(20, BACKWARD); // direito pra trás
    // speed[ESQUERDA] = DEFAULT_SPEED * (1 + decimalDaVelocidade);; // Aumenta velocidade do motor esquerdo
    // speed[DIREITA] = DEFAULT_SPEED * (1 - decimalDaVelocidade); // Reduz velocidade do motor direito
  } else if (lado == DIREITA) {
    motors.leftDrive(20, BACKWARD); // esquerdo pra trás
    motors.rightDrive(20, FORWARD); // direito pra frente 
    // speed[ESQUERDA] = DEFAULT_SPEED * (1 - decimalDaVelocidade); // Reduz velocidade do motor esquerdo
    // speed[DIREITA] = DEFAULT_SPEED * (1 + decimalDaVelocidade);; // Aumenta velocidade do motor direito
  }
}

/**
 * @brief Display function to show messages on the LCD
 * @param show 2D array of characters to display
 */
void display(char linha1[16], char linha2[16]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
}