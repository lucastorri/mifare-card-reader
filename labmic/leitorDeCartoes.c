#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <getopt.h>
#include <termios.h>
#include <time.h>
#include <sys/stat.h>

#define BYTES_NO_CARTAO 8
#define DEVICE "/dev/ttyS0"
#define BAUDRATE 9600
#define ARQUIVO_DE_USUARIOS "/root/labmic/usuarios"
#define ARQUIVO_LOG "/root/labmic/log"

void configuraSerial();
char lerSerial();
void validarCartao(char[]);

FILE *arquivoDeCartoes;
char id_cartao[BYTES_NO_CARTAO];
int gPortFd = -1;

int main() {
    configuraSerial();

    char c;
    while(1) {
        if ((c = lerSerial()) == 0x02) {
            int i = 0;
            while((c = lerSerial()) != 0x0D) {
                id_cartao[i++] = c;
            }
      
            validarCartao(id_cartao);
        }
    }
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

void validarCartao(char id_recebido[]) {
    FILE *arquivoDeCartoes = fopen(ARQUIVO_DE_USUARIOS, "r");
    FILE *arquivoLog = fopen(ARQUIVO_LOG, "a");
    chmod(ARQUIVO_LOG, S_IRWXU|S_IRGRP|S_IROTH);
    char linha[200];
    while (!feof(arquivoDeCartoes)) {
        fgets(linha, 200, arquivoDeCartoes);
        char *id = strtok(linha, "|");

        if (strcmp(id_recebido, id) == 0)  {
				char * nome = strtok(NULL, "|");
		    
		        fputs(id_recebido,arquivoLog);
		        fputs("|",arquivoLog);
		        fputs(nome, arquivoLog);
		        fputs("|", arquivoLog);

		        time_t rawtime;
		        struct tm * timeinfo;
		        time ( &rawtime );
		        timeinfo = localtime ( &rawtime );
			char * data = asctime(timeinfo);
			char * temp = strstr(data, "\n");
			*temp = (char) NULL;
		        fputs(data, arquivoLog);
                fputs("|" ,arquivoLog);
                
				fputs("YES", arquivoLog);
                fputs("\n", arquivoLog);
				fclose(arquivoDeCartoes);
		        fclose(arquivoLog);
				return;
        }
    }
								
		        fputs(id_recebido,arquivoLog);
		        fputs("|",arquivoLog);
		        fputs("?", arquivoLog);
		        fputs("|", arquivoLog);

		        time_t rawtime;
		        struct tm * timeinfo;
		        time ( &rawtime );
		        timeinfo = localtime ( &rawtime );
		        char * data = asctime(timeinfo);                                                                                                     
                        char * temp = strstr(data, "\n");                                                                                                    
                        *temp = (char) NULL;   
			 fputs(data, arquivoLog);
                fputs("|" ,arquivoLog);
		          
                
				fputs("NO", arquivoLog);
                fputs("\n", arquivoLog);
				fclose(arquivoDeCartoes);
		        fclose(arquivoLog);
				fclose(arquivoDeCartoes);
		        fclose(arquivoLog);				
}

