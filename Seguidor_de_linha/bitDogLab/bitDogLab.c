#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/regs/i2c.h"

// Escolha o I2C (i2c0 ou i2c1)
#define I2C_PORT           i2c1
#define I2C_PORT_IRQ       I2C1_IRQ
#define I2C_SDA_PIN        0       // GP4 = SDA
#define I2C_SCL_PIN        1       // GP5 = SCL
#define I2C_BAUDRATE       100000  // taxa qualquer, o escravo ignora
#define SLAVE_ADDRESS      0x08

// Buffer de usuário
static uint8_t rx_buffer[32];
static int rx_head = 0;

// Configura o I2C0 em modo escravo e habilita IRQ
void i2c_slave_init() {
    // Desabilita antes de configurar
    i2c_get_hw(I2C_PORT)->enable = 0;

    // Configura SDA/SCL
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Define o endereço do escravo
    i2c_get_hw(I2C_PORT)->sar = SLAVE_ADDRESS;

    // Limpa flags e FIFO
    i2c_get_hw(I2C_PORT)->clr_intr = 
        I2C_IC_CLR_INTR_BITS_MASK |
        I2C_IC_CLR_RX_UNDER_BITS_MASK |
        I2C_IC_CLR_RX_OVER_BITS_MASK |
        I2C_IC_CLR_TX_OVER_BITS_MASK |
        I2C_IC_CLR_RD_REQ_BITS_MASK |
        I2C_IC_CLR_TX_ABRT_BITS_MASK |
        I2C_IC_CLR_RX_DONE_BITS_MASK |
        I2C_IC_CLR_ACTIVITY_BITS_MASK |
        I2C_IC_CLR_STOP_DET_BITS_MASK |
        I2C_IC_CLR_START_DET_BITS_MASK |
        I2C_IC_CLR_GEN_CALL_BITS_MASK;

    // Habilita as interrupções que vamos usar
    uint32_t mask = 
        I2C_IC_INTR_RX_FULL_BITS_MASK   |  // dado recebido
        I2C_IC_INTR_RD_REQ_BITS_MASK     |  // mestre quer ler
        I2C_IC_INTR_STOP_DET_BITS_MASK;     // fim de transação
    i2c_get_hw(I2C_PORT)->intr_mask = mask;

    // Reabilita o I2C em modo escravo
    i2c_get_hw(I2C_PORT)->enable = I2C_IC_ENABLE_ENABLE_BITS;
    // Para escravo, SLAVE_DISABLE deve ficar em 0 (padrão)

    // Conecta handler e habilita IRQ no NVIC
    irq_set_exclusive_handler(I2C_PORT_IRQ, [](void){
        uint32_t status = i2c_get_hw(I2C_PORT)->intr_stat;
        
        // Mestre escreveu dados para o escravo?
        if (status & I2C_IC_INTR_RX_FULL_BITS_MASK) {
            // lê até 32 bytes ou até esvaziar FIFO
            while (i2c_get_hw(I2C_PORT)->status & I2C_IC_STATUS_RFNE_BITS) {
                uint8_t b = i2c_get_hw(I2C_PORT)->data_cmd;
                if (rx_head < sizeof(rx_buffer)) rx_buffer[rx_head++] = b;
            }
        }
        // Mestre solicitou leitura do escravo?
        if (status & I2C_IC_INTR_RD_REQ_BITS_MASK) {
            // envie um byte de resposta
            static const uint8_t response[] = { 0xAA, 0xBB, 0xCC };
            static size_t resp_idx = 0;
            uint8_t to_send = response[resp_idx++ % sizeof(response)];
            // TX FIFO
            i2c_get_hw(I2C_PORT)->data_cmd = to_send;
        }
        // Transação finalizada?
        if (status & I2C_IC_INTR_STOP_DET_BITS_MASK) {
            // limpe flag de STOP
            i2c_get_hw(I2C_PORT)->clr_intr = I2C_IC_CLR_STOP_DET_BITS_MASK;
            // aqui você pode processar o rx_buffer…
            rx_head = 0;
        }
        // Limpa as flags tratadas
        i2c_get_hw(I2C_PORT)->clr_intr = status;
    });
    irq_set_enabled(I2C_PORT_IRQ, true);
}

int main() {
    stdio_init_all();
    i2c_slave_init();
    printf("I2C slave iniciado no endereço 0x%02X\n", SLAVE_ADDRESS);

    while (1) {
        tight_loop_contents();
    }
    return 0;
}