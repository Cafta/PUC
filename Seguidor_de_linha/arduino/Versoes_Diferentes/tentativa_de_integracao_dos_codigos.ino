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
int8_t opcao = 1;
int8_t resultante = 0;
int8_t auxRes = 0;
int8_t posicao[2];
bool estadoLed = false;

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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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

int8_t botaoPressionado(int botao) {
  // Verifica se o botão foi pressionado
  if (digitalRead(botao) == LOW){ //&& solta == false) {

    int tempo = millis();
    while(millis() - tempo < 100){
      if(digitalRead(A0) == LOW && digitalRead(A1) == LOW){// && millis() - tempo <= 300){
        return 3;
      }
    }
    
    if(digitalRead(A0) == HIGH && digitalRead(A1) == LOW){// && flag == false){
    opcao++;
    if(opcao >= 4){
      opcao = 4;
    }
  }
  if(digitalRead(A0) == LOW && digitalRead(A1) == HIGH){// && flag == false){
    opcao--;
    if(opcao <= 1){
      opcao = 1;
    }
  }
  delay(150);
  return 1;
  }
  else if (digitalRead(botao) == HIGH){
    return 0;
  }
}

void decisao(){
  
  bool orientacao = false; // false DIREITA true ESQUERDA
  // CHEGOU
  
  if(posicao[0] > posicao[1]){ // esta virado para a direita
  orientacao = false;
  }
  else if (posicao[0] < posicao[1]){ // esta virado para a esquerda
  orientacao = true;
  }
  
  if (resultante == 0){ // CASO TENHA ESTEJA NO DESTINO 
    lcd.setCursor(0, 0);
    lcd.print("Opcoes: 1 2 3 4 ");
    while(true){
      botaoPressionado(A0);
      botaoPressionado(A1);
      
      if(botaoPressionado(A0) == 3 || botaoPressionado(A1) == 3){
        break;
      }
      
      if(opcao == 1){
        lcd.setCursor(0, 1);
        lcd.print("        ^       ");
      }
      if(opcao == 2){
        lcd.setCursor(0, 1);
        lcd.print("          ^     ");
      }
      if(opcao == 3){
        lcd.setCursor(0, 1);
        lcd.print("            ^   ");
      }
      if(opcao == 4){
        lcd.setCursor(0, 1);
        lcd.print("              ^ ");
      }
    }
    
    lcd.setCursor(0, 0);
    lcd.print("SAIU DO WHILE   ");
    
    posicao[0] = posicao[1];
    posicao[1] = opcao;
    
    if(posicao[0] == posicao[1]) { // CASO O ENGRACADINHO ESCOLHA COMO DESTINO A ESTACAO ATUAL
      decisao();
    }
    
    auxRes = posicao[0] - posicao[1];
    resultante = abs(auxRes); // quantas casas o carrinho precisara andar
    if(orientacao == true && posicao[0] > posicao[1] || orientacao == false && posicao[0] < posicao[1]){ // esta virado para esquerda, mas seu novo destino fica a direita
      lcd.setCursor(0, 0);
      lcd.print("ROTACIONANDO    ");
      delay(1000);
      // rotaciona180();
    }
  }
  else { // CASO AINDA NAO TENHA CHEGO NO DESTINO
    lcd.setCursor(0, 0);
    lcd.print("Seguindo para o ");
    lcd.setCursor(0, 1);
    lcd.print("destino");
    delay(500);
  }
  
  resultante--;
  estadoDeDecisao = ANDANDO;
  // modo = NAVEGACAO;
  
}

void decide() {
  Serial.println("Modo de decisão: ");
  if (estadoDeDecisao == ESTACAO) {
    decisao();
    return; // Não faz nada, espera decisão do BitDogLab
  } else if (estadoDeDecisao == ANDANDO) {
    andaLentamente(); 
    lerSensores(); 
    if (naLinha[ESQUERDA] == 1 && naLinha[DIREITA] == 1) {
      estadoDeDecisao = ESTACAO; 
    } else if (naLinha[ESQUERDA] == 0 && naLinha[DIREITA] == 0) {
      // Recupera última leitura dos sensores
      if (ultimaLeitura[ESQUERDA] == 1 && ultimaLeitura[DIREITA] == 0) {
        procurarLinha(ESQUERDA); // Procura linha à esquerda
      } else {
        procurarLinha(DIREITA); // Procura linha à direita
      }
      estadoDeDecisao = ANDANDO; // Retorna para o estado de decisão
      modo = NAVEGACAO; // Retorna para o modo de navegação
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

  Serial.print("Sensor E: "); 
  Serial.print(left.check());
  Serial.print("  Sensor D: "); 
  Serial.println(right.check());
}

void andaLentamente() {
  // Liga os motores com velocidade 10
  setMotor(TODOS, 10); // Liga o motor
  // anda por 100ms
  delay(DELAY_DE_RASTREAMENTO); // O suficiente para sair da inercia
  // Desliga os motores
  setMotor(TODOS, 0); // Desliga os motores
}

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

void vel_motor() {
  if (objetoNaVia()) {
    // Se o objeto estiver na frente, pare os motores
    speed[ESQUERDA] = 0;
    speed[DIREITA] = 0;
    //bitDogLab_set(ALARME); // Ativa alarme
  } else if (flag_alarme) {
    // Se o alarme estiver ativado e não houver objeto na frente, desative o alarme
    //bitDogLab_set(DESALARMAR); // Desativa alarme
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

bool objetonavia() {
  long duration;
  int distancia;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distancia = duration * 0.034 / 2;

  return distancia < limiteDistancia;
}

void power_speed(uint8_t roda, uint8_t velocidade){
  setMotor(roda, 80); // Liga o motor a 50% de potência para sair da inercia
  delay(5); // O suficiente para sair da inercia 
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

void rodaCarrinho(Movimento lado, uint8_t decimalDaVelocidade) {
  // Liga os motores para o lado especificado
  if (lado == ESQUERDA) {
    speed[ESQUERDA] = DEFAULT_SPEED * (1 + decimalDaVelocidade);; // Aumenta velocidade do motor esquerdo
    speed[DIREITA] = DEFAULT_SPEED * (1 - decimalDaVelocidade); // Reduz velocidade do motor direito
  } else if (lado == DIREITA) {
    speed[ESQUERDA] = DEFAULT_SPEED * (1 - decimalDaVelocidade); // Reduz velocidade do motor esquerdo
    speed[DIREITA] = DEFAULT_SPEED * (1 + decimalDaVelocidade);; // Aumenta velocidade do motor direito
  }
}

void piscaLed(){
  estadoLed = !estadoLed;
  digitalWrite(LED_BUILTIN, estadoLed);
  //delay(250);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (modo == NAVEGACAO) {

    int temporizador = millis();
    if (flag_alarme == true){ // ALARME
    lcd.setCursor(0, 0);
    lcd.print("Objeto na via!  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    if(millis() - temporizador >= 250){
      temporizador = millis();
      piscaLed();
    }

    navegar();
  }
  } else { // modo == DECISAO
    decide();
    //Serial.println("Entrou modo Decisao. Volte para o moda navegação apertando Btn0");
    //while (!botaoPressionado(BOTAO1_USUARIO));  // Espera usuário confirmar
    //modo = NAVEGACAO; // Retorna para o modo de navegação
  }
  delay(1);
}
