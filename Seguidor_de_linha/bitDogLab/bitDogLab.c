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
#define I2C_ADDR 0x08
#define VRY_PIN 27
#define VRX_PIN 26
#define SW_PIN 22
#define VRY 0
#define VRX 1

#define selecionar_opcao 1
//#define a_caminho 2

typedef struct __attribute__((packed)) {
    uint8_t command_id;
    uint8_t detail_id;
    uint8_t status_flag;
} Packet;

// STRUCTS definitions
int8_t mensagem_arduino(){
    int8_t buf[3];
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    i2c_read_blocking(I2C_PORT, I2C_ADDR, buf, sizeof(buf), false);
    //Packet *p = (Packet *)buf;
    if(buf[0] == 1){
        printf("digito 1 correto\n");
    }
    
    if(buf[1] == 2){
        printf("digito 2 correto\n");
    }

    if(buf[2] == 3){
        printf("digito 3 correto\n");
    }
}

int main()
{
    stdio_init_all();
    i2c_init(i2c1, 400000);

    while (true) {
        mensagem_arduino();
    }
}
