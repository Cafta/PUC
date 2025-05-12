/*
  ****** PUC CAMPINAS - Abr 2025 *******
      ENG. DE CONTROLE E AUTOMAÇÃO
        ** VEÍCULOS AUTÔNOMOS **
  
          COURRIER ELETRÔNICO
           -- MÓDULO MENU --

  GRUPO: CARLOS AMARAL
         GABRIEL MORELLI
         GUILHERME TALMAN
         MURILO TREVISAN
*/

#include <LiquidCrystal.h>

// Definições
#define SOBE 0
#define DESCE 1
#define NEUTRO 2
#define DIREITA 3
#define ESQUERDA 4
#define CIMA 5
#define BAIXO 6
#define eixo_X A0
#define eixo_Y A1
#define n_itens_menu 4
const char *menu[] = {
  "Mesa 1",
  "Mesa 2",
  "Mesa 3",
  "Mesa 4",
};

// Inicializa a biblioteca com os pinos conectados: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

// Variaveis Globais
int menu_selected[] = {0, 0, CIMA}; // {posição_atual, última_posição, posição_no_display}
int ultima_posicao_jostick = NEUTRO;

// void menu(int direcao) {
//   if (direcao == SOBE) {
//     lcd.setCursor(0, 0);
//     lcd.print("*");
//     lcd.setCursor(0, 1);
//     lcd.print(" ");
//   } else if (direcao == DESCE) {
//     lcd.setCursor(0, 0);
//     lcd.print(" ");
//     lcd.setCursor(0, 1);
//     lcd.print("*");
//   }
// }

void print_menu() {
  // menu_selected[] = {posição_atual, última_posição, posição_no_display}
  // só preciso da posicao_atual e posicao_no_display
  int selecionado = menu_selected[0];
  int posicao_display = menu_selected[2];

  if (posicao_display == 0) {
    lcd.setCursor(0, 0);    // Posiciona o cursor na coluna 3 da primeira linha
    lcd.print("* ");
    lcd.print(menu[selecionado]);   // Escreve na primeira linha
    lcd.setCursor(0, 1);    // Posiciona o cursor na coluna 3 da segunda linha
    lcd.print("  ");
    lcd.print(menu[selecionado + 1]);
  }
}

void menu_change(int direcao) {
  if (menu_selected[2] == CIMA) {  // a posição atual da seleção está emcima no display
    if (direcao == SOBE) {
      if (menu_selected[0] != 0) { // está subindo e não é o primeiro da fila 
      Serial.println("Este é o primeiro da fila, não dá para subir mais");
    } else {
      Serial.println("Não é o primeiro da fila, dá para subir mais");
      print_menu();
    }
  } else {  // a seleção atual está enbaixo no display


  }
}

void set_menu_position(int posicao_jostick) {
  // Serial.println(posicao);  // valor recebido do jostick
  if (posicao_jostick < 1400 && ultima_posicao_jostick != DESCE) {   // posicao é para baixo && verifica se mudou de posição ou está só repetindo
    ultima_posicao_jostick != DESCE;
    if (menu_selected[1] == ) {   // neste caso mudou a seleção do item
      menu_change(SOBE);
      Serial.println("Subindo");
    } if (posicao > 1500) {
      menu_change(DESCE);
      Serial.println("Descendo");
    }
    menu_selected[1] = menu_selected[0];
  }
}

void setup() {
  Serial.begin(19600);
  lcd.begin(16, 2); // Define que o display tem 16 colunas e 2 linhas
  
  print_menu(); // desenha o menu inicial
}

void loop() {
  int manopla_X = analogRead(eixo_X);
  // manopla_Y = analogic_read(eixo_Y); // Não utilizado
  set_menu_position(manopla_X);

}

