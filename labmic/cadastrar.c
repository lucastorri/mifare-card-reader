#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <getopt.h>
#include <termios.h>

#define BYTES_NO_CARTAO 8
#define DEVICE "/dev/ttyS0"
#define BAUDRATE 9600
#define ARQUIVO_DE_USUARIOS "/root/labmic/usuarios"

void configuraSerial();
char lerSerial();

FILE *arquivoDeCartoes;
char id_cartao[BYTES_NO_CARTAO];
int gPortFd = -1;

int main() {
    configuraSerial();

    char c;
    puts("Por favor, passe o cartao");
    while(1) {
        if ((c = lerSerial()) == 0x02) {
            int i = 0;
            while((c = lerSerial()) != 0x0D) {
                id_cartao[i++] = c;
            }
            
            printf("Qual seu nome? ");
            char nome[200];
            i = 0;
            while ((c = getchar()) != '\n') {
                nome[i++] = c;
                printf("%c", c);
            }

            FILE *arquivo = fopen(ARQUIVO_DE_USUARIOS, "a");
            fputs(id_cartao, arquivo);
            fputs("|", arquivo);
            fputs(nome, arquivo);
            fputs("|", arquivo);
            fputs("\n", arquivo);
            fflush(arquivo);
            fclose(arquivo);

            puts("\nCadastro criado com sucesso!");
            printf("Para completa-lo por favor insira uma foto sua de 120x120 pixels, com nome %s.jpg, na pasta /root/labmic/pics\n", id_cartao);
            return 0;
        }
    }
    return 0;
}

void configuraSerial() {
    struct termios attr;

    if ((gPortFd = open(DEVICE, O_RDWR | O_EXCL)) < 0) {
        printf("Unable to open serial port '%s': %s\n", DEVICE, strerror(errno));
        exit(1);
    }

    if (tcgetattr(gPortFd, &attr) < 0) {
        printf("Call to tcgetattr failed: %s\n", strerror(errno));
        exit(1);
    }

    attr.c_iflag = 0;
    attr.c_oflag = 0;
    attr.c_cflag = CLOCAL | CREAD | CS8;
    attr.c_lflag = 0;
    attr.c_cc[ VTIME ] = 0;
    attr.c_cc[ VMIN ] = 1;

    cfsetispeed(&attr, BAUDRATE);
    cfsetospeed(&attr, BAUDRATE);

    if (tcsetattr(gPortFd, TCSAFLUSH, &attr) < 0) {
        printf("Call to tcsetattr failed: %s\n", strerror(errno));
        exit(1);
    }

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    {
        struct termios tio;

        if (tcgetattr(fileno(stdin), &tio) < 0) {
            printf("Unable to retrieve terminal settings: %s\n", strerror(errno));
            exit(1);
        }

        tio.c_lflag &= ~( ICANON | ECHO );
        tio.c_cc[VTIME] = 0;
        tio.c_cc[VMIN] = 1;

        if (tcsetattr(fileno(stdin), TCSANOW, &tio) < 0) {
            printf("Unable to update terminal settings: %s\n", strerror(errno));
            exit(1);
        }
    }
}

char lerSerial() {
    char ch;
    int bytesRead;

    if ((bytesRead = read(gPortFd, &ch, 1)) < 0) {
        printf("Serial port read failed: %s\n", strerror(errno));
        exit(1);
    }
    
    return ch;
}
