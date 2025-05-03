#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font6x8_com_pos_espaco.h"
#include <string.h>
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments

#define SSD1306_I2C_ADDR 0x3C
#define I2C_PORT i2c0
#define I2C_SDA 14
#define I2C_SCL 15
#define VRY_PIN 27
#define VRX_PIN 26
#define SW_PIN 22
#define VRY 0
#define VRX 1

#define selecionar_opcao 1
//#define a_caminho 2

// STRUCTS definitions
struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

//int8_t escolha = 1;
int8_t option = 1;
int8_t pressionado = 0;
int8_t estado_carrinho = 1;

bool menos_um = false;
bool mais_um = false;
bool escolha_1 = false;
bool escolha_2 = false;
bool escolha_3 = false;
bool escolha_4 = false;
bool escolha = true;
bool teste = true;


// Global Buffer for the SSD1306 Display 
uint8_t ssd[ssd1306_buffer_length];

void OLED_init();
void joystick_init();
void OLED_draw_char(uint8_t *ssd, int16_t x, int16_t y, uint8_t character, bool invert) ;
void oled_draw_string(uint8_t *ssd, int16_t x, int16_t y, const char *string, bool invert);
void clear_ssd1306_i2c();
void DISPLAY(); 

int main()
{
    OLED_init();
    joystick_init();

    while (true) {
        DISPLAY();
        sleep_ms(10);
    }
}

void OLED_init() {
    // Starting the Display
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); // Start the I2C at 400kHz (set in ssd1306_i2c.h)
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // SCL
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();  // It is in ssd1306_i2c.c
    calculate_render_area_buffer_length(&frame_area); // in ssd1306_i2c.c
    clear_ssd1306_i2c();   // Resets the display
}

// Resets the display
void clear_ssd1306_i2c() {
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

// Draw a single character on the display
void OLED_draw_char(uint8_t *ssd, int16_t x, int16_t y, uint8_t character, bool invert) {
    int fb_idx = (y / 8) * 128 + x;
    for (int i = 0; i < 6; i++) {
        ssd[fb_idx++] = invert? ~FONT6x8[character - 0x20][i] : FONT6x8[character - 0x20][i];
    }
}

// Draw a string by calling OLED_draw_char() for each character
void oled_draw_string(uint8_t *ssd, int16_t x, int16_t y, const char *string, bool invert) {
    if (x > ssd1306_width - 6 || y > ssd1306_height - 8) {
        return;
    }
    x = (x == 0) ? 1: x;
    while (*string) {
        OLED_draw_char(ssd, x, y, *string++, invert);
        x += 6;
    }
}

void joystick_init() {
    // Setting the Joystick
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);
}

// Apply corrections to joystick axes 
int remap(int x, int in_min, int in_max, int metade, int out_min, int out_max) {
    int ded_zone = 200;
    if (x > (metade - ded_zone) && x < (metade + ded_zone)) {
        return 0;
    } else if (x < metade) {
        return (metade - x) * out_min / (metade - in_min);
    } else if (x > metade) {
        return (x-metade) * out_max / (in_max-metade);
    }    
}

int8_t sw(int8_t pressionado) {
    int8_t estado_sw = gpio_get(SW_PIN);
    bool ligado = false;

    if(estado_sw > 0 && pressionado != 2){
        pressionado = 2;
        return 1;
    }
    else{
        return 0;
    }
}

int posicao_analogico(){
    adc_select_input(VRX);
    int16_t x = adc_read();
    x = remap(x, 18, 4081, 2010, -2048, 2048);
    adc_select_input(VRY);
    int16_t y = adc_read();
    y = remap(y, 18, 4082, 1937, -2048, 2048);
    return x;
}

int8_t escolhendo(int8_t option) {

    int16_t x = posicao_analogico();;

    if(x < -1524 && menos_um == false){
        sleep_ms(250);
        option -= 1;
        menos_um = true;
    }
    else if(x > 1524 && mais_um == false){
        sleep_ms(250);
        option += 1;
        mais_um = true;
    }

    if(option > 4){
        option = 4;
    }
    else if(option < 1){
        option = 1;
    }
    return option;
}

int8_t estado(bool navegacaoDecisao, bool alarme){
    //I2C com o ARDUINO
    if (navegacaoDecisao == true){
        return 1;
    }
    else{
        return 0;
    }
}

void DISPLAY() {
    int8_t estado_carrinho = estado(gpio_get(19), gpio_get(18));
    if (estado_carrinho == selecionar_opcao){
        char buffer[22];
        char cabecalho[] = "   Mova o joystick   ";
        oled_draw_string(ssd, 0, 16, cabecalho, false);
        char sub_cabecalho[] = "     <--  o  -->     ";
        oled_draw_string(ssd, 0, 32, sub_cabecalho, false);
        render_on_display(ssd, &frame_area);
        while (sw(pressionado) == 1) {
            mais_um = false;
            menos_um = false;
            option = escolhendo(option);
            if (option == 1 && escolha_1 == false){
                char buffer[22];
                char cabecalho[] = "Onde? Op1 Op2 Op3 Op4";
                snprintf(buffer, sizeof(buffer), "       o             ");
                memset(ssd, 0, ssd1306_buffer_length);
                oled_draw_string(ssd, 0, 16, cabecalho, false);
                oled_draw_string(ssd, 0, 32, buffer, false);
                render_on_display(ssd, &frame_area);
                escolha_2 = false;
                escolha_3 = false;
                escolha_4 = false;
                escolha_1 = true;
            }
            else if(option == 2 && escolha_2 == false){
                char buffer[22];
                char cabecalho[] = "Onde? Op1 Op2 Op3 Op4";
                snprintf(buffer, sizeof(buffer), "           o         ");
                memset(ssd, 0, ssd1306_buffer_length);
                oled_draw_string(ssd, 0, 16, cabecalho, false);
                oled_draw_string(ssd, 0, 32, buffer, false);
                render_on_display(ssd, &frame_area);
                escolha_1 = false;
                escolha_3 = false;
                escolha_4 = false;
                escolha_2 = true;
            }
            else if(option == 3 && escolha_3 == false){
                char buffer[22];
                char cabecalho[] = "Onde? Op1 Op2 Op3 Op4";
                snprintf(buffer, sizeof(buffer), "               o     ");
                memset(ssd, 0, ssd1306_buffer_length);
                oled_draw_string(ssd, 0, 16, cabecalho, false);
                oled_draw_string(ssd, 0, 32, buffer, false);
                render_on_display(ssd, &frame_area);
                escolha_1 = false;
                escolha_2 = false;
                escolha_4 = false;
                escolha_3 = true;
            }
            else if(option == 4 && escolha_4 == false){
                char buffer[22];
                char cabecalho[] = "Onde? Op1 Op2 Op3 Op4";
                snprintf(buffer, sizeof(buffer), "                   o ");
                memset(ssd, 0, ssd1306_buffer_length);
                oled_draw_string(ssd, 0, 16, cabecalho, false);
                oled_draw_string(ssd, 0, 32, buffer, false);
                render_on_display(ssd, &frame_area);
                escolha_1 = false;
                escolha_2 = false;
                escolha_3 = false;
                escolha_4 = true;
            }
        }
        estado_carrinho = 0;
        // FALTA ESCREVER ATRAVES DO I2C PARA ONDE O CARRINHO DEVE IR
    }
    else { // FALTA RECEBER A INFORMACAO DO ARDUINO PARA SUBSTITUIR O TEMPO PELA LOGICA QUE INDICARA CHEGADA AO DESTINO
        char cabecalho[] = "     A Caminho...    ", escolha;
        memset(ssd, 0, ssd1306_buffer_length);
        oled_draw_string(ssd, 0, 16, cabecalho, false);
        render_on_display(ssd, &frame_area);
        sleep_ms(5000);
        estado_carrinho = 1;
    }
}