#include <cerrno>
#include <cstring>

#include <iostream>
#include <chrono>
#include <thread>
#include <fcntl.h>      // open, O_RDWR, O_NOCTTY, etc.
#include <unistd.h>     // read, write, close
#include <termios.h>    // termios, tcgetattr, cfsetispeed, etc.
#include <opencv2/core/cuda.hpp>   // só para inicializar CUDA
#include "contador.hpp"

int port_init(const char* porta);
int serial = -1;
const char* port = "/dev/ttyUSB0";

int main() {
    // Se houver ao menos uma GPU CUDA, mostre no console
    int gpus = cv::cuda::getCudaEnabledDeviceCount();
    if (gpus > 0) {
        std::cout << "CUDA disponivel! GPUs: " << gpus << std::endl;
        cv::cuda::printShortCudaDeviceInfo(0);
    } else {
        std::cout << "OpenCV compila sem CUDA OU gpu nao detectada." << std::endl;
    }
    serial = port_init(port);
    if (serial < 0) return 1;

    iniciarContador();  // Inicia thread em segundo plano

    while (true) {
        float relacao = relacaoCarros();
        std::cout << "Relacao de carros: " << relacao << std::endl;

        // MUDAR O SEMÁFORO ALEATOREAMENTE SÓ PARA TESTAR:
        // Envia 1 byte (exemplo: S1:R e S2:Y)
        uint8_t value = 0b100010;
        write(serial, &value, 1);

        // Aguarda resposta
        char resposta;
        int n = read(serial, &resposta, 1);
        if (n > 0)
            std::cout << "Recebido do Arduino: " << resposta << std::endl;
        else
            std::cout << "Nenhuma resposta recebida\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
    return 0;
}


int port_init(const char* porta) {
    // Abre a porta serial
    int serial = open(porta, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial < 0) {
        std::cerr << "open() falhou: " << strerror(errno) << "\n";
        return -1;
    }

    // Configura a porta
    termios tty;
    if (tcgetattr(serial, &tty) != 0) {
        std::cerr << "Erro ao obter atributos da porta" << std::endl;
        close(serial);
        return -1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8 bits
    tty.c_iflag &= ~IGNBRK;                         // desabilita break
    tty.c_lflag = 0;                                // modo raw
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;                            // leitura bloqueante
    tty.c_cc[VTIME] = 1;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // desabilita controle de fluxo
    tty.c_cflag |= (CLOCAL | CREAD);                // habilita leitura
    tty.c_cflag &= ~(PARENB | PARODD);              // sem paridade
    tty.c_cflag &= ~CSTOPB;                         // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // sem controle de fluxo RTS/CTS

    if (tcsetattr(serial, TCSANOW, &tty) != 0) {
        std::cerr << "Erro ao configurar a porta" << std::endl;
        close(serial);
        return -1;
    }
    return serial;
}
