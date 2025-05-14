#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 1, 4, 10, 9);

int opcao = 1;
int resultante = 0;
int auxRes = 0;
int posicao[2];
bool estadoLed = false;
int aux = 0;

void setup() {

  lcd.begin(16,2);
  
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  
  posicao[0] = 0;
  posicao[1] = 1;
  
  lcd.setCursor(0, 0);
  lcd.print("Modo Calibracao.");
  delay(1500);
  lcd.setCursor(0, 0);
  lcd.print("Fora da linha   ");
  lcd.setCursor(0, 1);
  lcd.print("aperte o btn0!  ");
  
  while(botaoPressionado(A0) == 0){
    /*left.setBGLevel();
    //right.setBGLevel();*/
  }
  
  delay(1000);
  
  lcd.setCursor(0, 0);
  lcd.print("Em cima da linha");
  lcd.setCursor(0, 1);
  lcd.print("aperte o btn1   ");
  
  while(botaoPressionado(A1) == 0){
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
    while(true){//digitalRead(A0) == HIGH || digitalRead(A1) == HIGH){//sair_do_while == false){//)digitalRead(A0) == 0 || digitalRead(A1) == 0){
      botaoPressionado(A0);
      botaoPressionado(A1);
      
      /*if(digitalRead(A0) == LOW || digitalRead(A1) == LOW){
        int tempo = millis();
        while(millis() - tempo < 300){
          if(digitalRead(A0) == LOW && digitalRead(A1) == LOW){// && millis() - tempo <= 300){
            break;
          }
        }
      }*/
      
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
  // modo = NAVEGACAO;
  // estadoDeDecisao = ANDANDO;
  
}
    
void loop() {
  
  if(aux == 0){ // CHAMAR A FUNCAO DECIDE E TROCAR O IF PARA MODO == DECISAO
    decisao();
    aux = 1;
  }
  else if(aux == 1){ // CHAMAR A FUNCAO NAVEGAR E TROCAR O IF PARA MODO == NAVEGACAO
    lcd.setCursor(0, 0);
    lcd.print("NAVEGANDO!      ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(2000);
    aux = 0;
  }
}
