#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "contador.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

int via = 0;
int fd = -1;
#define arduino "/dev/ttyUSB0"

//  DECLARAÇÃO DE FUNÇÕES
void initSerial(const char* device = "/dev/ttyUSB0");

auto calculaTempo() -> std::chrono::steady_clock::time_point {
    auto retorno = std::chrono::steady_clock::now();
    float ajuste = 60 * (relacaoCarros() + 1);
    if (via == 1) ajuste = 120 - ajuste;
    retorno += std::chrono::seconds(static_cast<int>(ajuste));
    return retorno;
}

/**
 * Verifica se tem carro na via ativa (0 ou 1)
 * Retorna true ou false
 */
bool naoTemCarro() {
    if ((via == 0 && std::fabs(relacaoCarros() - 1.0f) < 0.05f) ||     // -1 indica que só tem carro na via 1 
        (via == 1 && std::fabs(relacaoCarros() + 1.0f) < 0.05f)) {     // 1 indica que só tem carro na via 0 
        return true;                                                    // <0.05f é uma tolerância para não dar erro de float arredondado
    }
    return false;
}

void colocaSinal(uint8_t frame) {
// TROCA OS SINAIS FÍSICOS NO ARDUINO
    char buffer[4];
    int len = snprintf(buffer, sizeof(buffer), "%u\n", frame);
    write(fd, buffer, len);
    tcdrain(fd);

    // PENAS EXIBIÇÃO NO SERIAL MONITOR
    std::string sinal_v0;
    std::string sinal_v1;
    sinal_v0 = (frame & 0b001000) ? "Verde" : 
               (frame & 0b010000) ? "amarelo" : "vermelho";
    sinal_v1 = (frame & 0b000001) ? "Verde" : 
               (frame & 0b000010) ? "amarelo" : "vermelho";
    std::cout << "Azul: " << sinal_v0 << " | Vermelho: " << sinal_v1 << std::endl;
}

void trocaSinal() {
    using namespace std::chrono;
    std::cout << " " << std::endl;

    if (via == 1) {     // Esta via é a ativa, a que vai ter que ficar verde. 
                        // Aqui ele não checa se já está aberta, então essa lógica tem que vir pronta
        colocaSinal(0b100010); 
        std::this_thread::sleep_for(seconds(6));  // espera no amarelo
        colocaSinal(0b100100); 
        std::this_thread::sleep_for(seconds(2));  // espera com os dois vermelhos para evitar acidentes
        colocaSinal(0b001100); // fica verde.
    } else if (via == 0) { 
        colocaSinal(0b010100); 
        std::this_thread::sleep_for(seconds(6));  // espera no amarelo
        colocaSinal(0b100100); 
        std::this_thread::sleep_for(seconds(2));  // espera com os dois vermelhos para evitar acidentes
        colocaSinal(0b100001); // fica verde.
    }
}

int main() {
    using namespace std::chrono;
    // steady_clock::time_point deadLine;
    auto deadLine = steady_clock::now() + seconds(2); 

    std::cout << "####  INICIANDO SISTEMA  #### " << std::endl;
    iniciarContador();  // Inicia a thread de contagem
    initSerial(arduino);  // Inicia a comunicação serial com o arduino
    std::cout << "Carregando.";
    while (steady_clock::now() < deadLine) 
    {
        std::cout << ".";
        std::this_thread::sleep_for(milliseconds(100));
    }
    std::cout << " " << std::endl;

    // coloca um valor para o sinal inicial
    colocaSinal(0b100001);
    std::cout << " " << std::endl;

    // INICIANDO O FLUXOGRAMA:
    while (true)
    {
        deadLine = calculaTempo();   // Pega o tempo inicial - daqui para frente só pode diminuir, nunca aumentar

        while(steady_clock::now() < deadLine) {   // Mantém em loop até acabar o tempo
            auto estadoAtual = calculaTempo();
            if (estadoAtual < deadLine) {
                auto reducao_ms = std::chrono::duration_cast<std::chrono::milliseconds>(deadLine - estadoAtual).count();
                auto tempo_faltando_s = std::chrono::duration_cast<std::chrono::seconds>(estadoAtual - std::chrono::steady_clock::now()).count();

                std::cout << "⏱️ Tempo reduzido em: " << reducao_ms << " ms. ";
                std::cout << "Faltando " << tempo_faltando_s << " segundos para a troca." << std::endl;
                deadLine = estadoAtual;  // diminui o tempo se a relação de carros assim determinar (nunca aumenta!!)
                
            }
        }

        via = via == 0? 1 : 0;   // Acabou o tempo da via ativa, troca de via agora

        if (naoTemCarro()) {      // não tem carro na via que iria abrir?
            via = via == 0? 1 : 0;   // retorna para a via que já estava aberta
        } else {
            trocaSinal();
        }

    }

    close(fd);  // fecha a porta de comunicação com o arduino
    return 0;
}

void initSerial(const char* device) {
    fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Erro ao abrir porta serial");
        exit(1);
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) {
        perror("Erro ao obter atributos da porta serial");
        exit(1);
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &tty);
}