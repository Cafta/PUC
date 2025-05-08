#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/regs/i2c.h"

#define I2C_PORT       i2c0
#define SDA_PIN        0       // GP4 = SDA
#define SCL_PIN        1       // GP5 = SCL
#define SLAVE_ADDRESS  0x08

int main() {
    // 1) Inicializa UART (para debug, opcional)
    stdio_init_all();
    //printf("I2C escravo inicializando em 0x%02X...\n", SLAVE_ADDRESS);

    // 2) Configura pinos SDA/SCL com pull-ups
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // 3) Inicializa o módulo I2C0 (baudrate arbitrário; o escravo ignora)
    i2c_init(I2C_PORT, 100000);

    // 4) Coloca em modo escravo no endereço SLAVE_ADDRESS
    i2c_set_slave_mode(I2C_PORT, true, SLAVE_ADDRESS);

    // 5) Habilita apenas as interrupções de “RD_REQ” e “STOP_DET”
    //    (RD_REQ = mestre pediu leitura; STOP_DET = fim de transação)
    i2c_get_hw(I2C_PORT)->intr_mask =
        I2C_IC_INTR_RD_REQ_BITS_MASK   |
        I2C_IC_INTR_STOP_DET_BITS_MASK;

    // 6) Loop principal: fica “pollando” as flags de interrupção
    while (true) {
        uint32_t status = i2c_get_hw(I2C_PORT)->intr_stat;

        // Mestre solicitou leitura?
        if (status & I2C_IC_INTR_RD_REQ_BITS_MASK) {
            const char msg[] = "resposta";
            // Enfileira cada byte no TX FIFO
            for (size_t i = 0; i < sizeof(msg) - 1; ++i) {
                i2c_get_hw(I2C_PORT)->data_cmd = msg[i];
            }
        }
    }
    // Limpa flag de STOP (para poder recomeçar próxima transação)
    if (status & I2C_IC_INTR_STOP_DET_BITS_MASK) {
        i2c_get_hw(I2C_PORT)->clr_intr = I2C_IC_CLR_STOP_DET_BITS_MASK;
    }

    // Limpa quaisquer outras flags ativas
    i2c_get_hw(I2C_PORT)->clr_intr = status;

    return 0;
}
