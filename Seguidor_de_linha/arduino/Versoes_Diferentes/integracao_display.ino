#include <LiquidCrystal.h>

enum Movimento { ESQUERDA, DIREITA, TODOS, PARADO, FRENTE }; // atribui automaticamente 0, 1, 2, 3
enum Modos { NAVEGACAO, DECISAO }; // Modo de navegação
//enum Comandos { ALARME, DESALARMAR, ERRO }; // Comandos para o bitDogLab
enum Estados { ESTACAO, ANDANDO }; // Estados de decisão

Modos modo = DECISAO;
Estados estadoDeDecisao = ESTACAO;

LiquidCrystal lcd(12, 11, 1, 4, 10, 9);

uint8_t ultimaVelocidade[] = {0, 0}; // {esq, dir} | Última velocidade dos motores 
uint8_t ultimaLeitura[] = {0, 0}; // {esq, dir} | Última leitura dos sensores
uint8_t naLinha[] = {0, 0}; // {esq, dir} | 0 = fora da linha, 1 = na linha
uint8_t speed[] = {0, 0}; // {esq, dir} | velocidade dos motores
uint8_t flag_parado[] = {0, 0}; // Flag para indicar se o motor está parado
uint8_t flag_alarme = 0; // Flag para indicar se o alarme está ativado
int leftValue;  // variable to store the left sensor value
int rightValue;  // variable to store the right sensor value

int limiteDistancia = 50;
int opcao = 1;
bool aperta = true;
bool solta = false;
int aux = 0;

void setup() {

  lcd.begin(16,2);
  //Serial.begin(9600);
  
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  
  lcd.setCursor(0, 0);
  lcd.print("Modo Calibracao.");
  delay(1500);
  lcd.setCursor(0, 0);
  lcd.print("Fora da linha   ");
  lcd.setCursor(0, 1);
  lcd.print("aperte o btn0!  ");
  
  while(botaoPressionado(A0) == 1){
    /*left.setBGLevel();
    //right.setBGLevel();*/
  }
  
  delay(1000);
  
  lcd.setCursor(0, 0);
  lcd.print("Em cima da linha");
  lcd.setCursor(0, 1);
  lcd.print("aperte o btn1   ");
  
  while(botaoPressionado(A1) == 1){
  /*left.setDetectLevel();
  right.setDetectLevel();*/
  }
    
  lcd.setCursor(0, 0);
  lcd.print("Calibracao      ");
  lcd.setCursor(0, 1);
  lcd.print("finalizada!     ");
}

int8_t botaoPressionado(int botao) {
  // Verifica se o botão foi pressionado
  /*if (digitalRead(botao) == LOW) {
  	if(digitalRead(A0) == HIGH && digitalRead(A1) == LOW){// && flag == false){
      opcao++;
      delay(250);
      return 1;
      //flag = true;
    }
    if(digitalRead(A0) == LOW && digitalRead(A1) == HIGH){// && flag == false){
      opcao--;
      delay(250);
      return 1;
      //flag = true;
    }
  }
  else{
    return 0;
  }*/
  if (digitalRead(botao) == LOW) {
    if(digitalRead(A0) == HIGH && digitalRead(A1) == LOW){// && flag == false){
      
      opcao--;
      if(opcao <= 1){
        opcao = 1;
      }
      //flag = true;
    }
    if(digitalRead(A0) == LOW && digitalRead(A1) == HIGH){// && flag == false){
      opcao++;
      if(opcao >= 4){
        opcao = 4;
      }
      //flag = true;
    }
    delay(100); // Debounce delay
    if (digitalRead(botao) == LOW) {
      return 1; // Botão pressionado
    }
  }
  return 0; // Botão não pressionado
}

bool objetonavia() {
  long duration;
  int distancia;

  digitalWrite(3, LOW);
  delayMicroseconds(2);
  digitalWrite(3, HIGH);
  delayMicroseconds(10);
  digitalWrite(3, LOW);

  duration = pulseIn(2, HIGH);
  distancia = duration * 0.034 / 2;
  
  /*Serial.print("Distancia detectada");
  Serial.println(distancia);*/

  return distancia < limiteDistancia;
}

void alarme(){
  lcd.setCursor(0, 0);
  lcd.print("Obstaculo na via");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  while(objetonavia() == true){
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }
}

void vel_motor() {
  if (objetoNaVia()) {
    // Se o objeto estiver na frente, pare os motores
    speed[ESQUERDA] = 0;
    speed[DIREITA] = 0;
    alarme(); // Ativa alarme
  } /*else if (flag_alarme) {
    // Se o alarme estiver ativado e não houver objeto na frente, desative o alarme
    bitDogLab_set(DESALARMAR); // Desativa alarme
    flag_alarme = 0; // Desativa alarme
  }*/
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

void decisao(){
  lcd.setCursor(0, 0);
  lcd.print("Opcoes: 1 2 3 4 ");
  while(digitalRead(A0) == LOW || digitalRead(A1) == LOW){//sair_do_while == false){//)digitalRead(A0) == 0 || digitalRead(A1) == 0){
  	botaoPressionado(A0);
    botaoPressionado(A1);
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
}

void decide(){
  if (estadoDeDecisao == ESTACAO) {
    decisao();// Não faz nada, espera decisão do BitDogLab
  }
  else if (estadoDeDecisao == ANDANDO) {
    andaLentamente(); 
    lerSensores(); 
    if (naLinha[ESQUERDA] == 1 && naLinha[DIREITA] == 1) {
      estadoDeDecisao = ESTACAO; 
    } 
    else if (naLinha[ESQUERDA] == 0 && naLinha[DIREITA] == 0) {
      // Recupera última leitura dos sensores
      if (ultimaLeitura[ESQUERDA] == 1 && ultimaLeitura[DIREITA] == 0) {
        procurarLinha(ESQUERDA); // Procura linha à esquerda
      } 
      else {
        procurarLinha(DIREITA); // Procura linha à direita
      }
      estadoDeDecisao = ANDANDO; // Retorna para o estado de decisão
      modo = NAVEGACAO; // Retorna para o modo de navegação
    }
  }
}

void navegar() {
  lerSensores();

  /*Serial.print("  Sensor E: ");
  Serial.print(naLinha[ESQUERDA]); 
  Serial.print("  Sensor D: ");
  Serial.println(naLinha[DIREITA]);*/

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
    
void loop() {
  if(modo == NAVEGACAO){
    navegacao();
    //modo = DECISAO; // condicao de chegada em uma estacao
  }
  else if(modo == DECISAO){
    decisao();
    //modo = NAVEGACAO; // condicao de finalizacao de escolha de endereco
  }
  delay(100);
}