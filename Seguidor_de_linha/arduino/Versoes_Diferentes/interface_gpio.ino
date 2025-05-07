#define ALARME 13
#define NAVEGACAO 12
//#define DECISAO 8

// CARRINHO SEMPRE COMECA NA POSICAO 1
bool auxOP1 = true;
bool auxOP2 = false;
bool auxOP3 = false;
bool auxOP4 = false;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(ALARME, OUTPUT);
  pinMode(NAVEGACAO, OUTPUT);
  //pinMode(DECISAO, OUTPUT);
  digitalWrite(ALARME, LOW);
  digitalWrite(NAVEGACAO, LOW);
  //digitalWrite(DECISAO, HIGH);
}

void loop() {
  // Pode atualizar o vetor dinamicamente, se quiser
  //bool 
  /*if(analogico_0 >= 570){
    //Serial.println("ENTRADA A0 DETECTOU HIGH");
    aux = 
  }
  if(analogico_1 >= 570){
    //Serial.println("ENTRADA A1 DETECTOU HIGH");
  }*/

  // CONSIDERANDO QUE SAIMOS SEMPRE DO OPCAO 1, PORQUE SO CONSEGUIMOS CONTAR AS PASSAGENS PELAS OUTRAS OPCOES PARA SABER ONDE O CARRINHO ESTA
  if(analogRead(A0) < 570 && analogRead(A1) < 570 && auxOP1 == false && digitalRead(NAVEGACAO) == LOW){
    auxOP2 = false;
    auxOP3 = false;
    auxOP4 = false;
    Serial.println("ESCOLHA: Opcao 1");
    auxOP1 = true;
    digitalWrite(NAVEGACAO, HIGH);
  }
  else if(analogRead(A0) < 570 && analogRead(A1) >= 570 && auxOP2 == false && digitalRead(NAVEGACAO) == LOW){
    auxOP1 = false;
    auxOP3 = false;
    auxOP4 = false;
    Serial.println("ESCOLHA: Opcao 2");
    auxOP2 = true;
    digitalWrite(NAVEGACAO, HIGH);
  }
  else if(analogRead(A0) >= 570 && analogRead(A1) < 570 && auxOP3 == false && digitalRead(NAVEGACAO) == LOW){
    auxOP1 = false;
    auxOP2 = false;
    auxOP4 = false;
    Serial.println("ESCOLHA: Opcao 3");
    auxOP3 = true;
    digitalWrite(NAVEGACAO, HIGH);
  }
  else if(analogRead(A0) >= 570 && analogRead(A1) >= 570 && auxOP4 == false && digitalRead(NAVEGACAO) == LOW){
    auxOP1 = false;
    auxOP2 = false;
    auxOP3 = false;
    Serial.println("ESCOLHA: Opcao 4");
    auxOP4 = true;
    digitalWrite(NAVEGACAO, HIGH);
  }
  //bool navegacao = digitalRead(NAVEGACAO);

  if(digitalRead(NAVEGACAO) == HIGH){ // FALTA A LOGICA QUE FARA O CARRINHO PARAR NA OPCAO ESCOLHIDA
    delay(5000);
    digitalWrite(NAVEGACAO, LOW);
  }

  delay(100);
}
