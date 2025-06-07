#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static bool set_interface_attribs(int fd, speed_t baud)
{
    termios tty{};
    if (tcgetattr(fd, &tty) != 0) { perror("tcgetattr"); return false; }

    cfmakeraw(&tty);
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | CSTOPB);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 20;

    return tcsetattr(fd, TCSANOW, &tty) == 0;
}

int main()
{
    const char *port = "/dev/ttyUSB0";  // altere se necessário
    printf("Abrindo %s…\n", port);
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) { perror("open"); return 1; }

    if (!set_interface_attribs(fd, B9600)) return 1;

    // evita reset do Arduino
    int mcs; ioctl(fd, TIOCMGET, &mcs);
    mcs &= ~(TIOCM_DTR | TIOCM_RTS);
    ioctl(fd, TIOCMSET, &mcs);

    puts("Esperando bootloader terminar (4 s)…");
    sleep(4);
    tcflush(fd, TCIOFLUSH);

    // Sequência: acende 1 LED por vez (valores decimais de 32 a 1)
    const uint8_t sequencia[] = {
        0b100000,  // P1_R (32)
        0b010000,  // P1_Y (16)
        0b001000,  // P1_G (8)
        0b000100,  // P2_R (4)
        0b000010,  // P2_Y (2)
        0b000001   // P2_G (1)
    };

    for (uint8_t val : sequencia)
    {
        char buffer[4];
        int len = snprintf(buffer, sizeof(buffer), "%u\n", val);
        write(fd, buffer, len);
        tcdrain(fd);

        printf("Enviado: %u\n", val);
        usleep(500000);  // 500 ms
    }

    puts("Fim do teste.");
    close(fd);
}
