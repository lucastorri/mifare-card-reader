#include <stdio.h>
#include <string.h>

#define LIMITE_POR_PAGINA 10
#define ARQUIVO_DE_LOG "/root/labmic/log"

void parseAndPrint(FILE *f, char linha[]);
        
int main() {
    char linha[300];
    
    FILE *arquivoDeLog = fopen(ARQUIVO_DE_LOG, "r");
    int numeroDeLinhas = 0;
    fgets(linha, 300, arquivoDeLog);
    while (!feof(arquivoDeLog)) {
        fgets(linha, 300, arquivoDeLog);
        if (strlen(linha) > 5) numeroDeLinhas++;
    }

    if (numeroDeLinhas > 0) {

        fseek(arquivoDeLog, 0, 0);
        if (numeroDeLinhas > LIMITE_POR_PAGINA) {
            int i;
            for (i = 0; i < numeroDeLinhas - LIMITE_POR_PAGINA; i++) {
                fgets(linha, 300, arquivoDeLog);
            }
        }

        fgets(linha, 300, arquivoDeLog);
        parseAndPrint(arquivoDeLog, linha);
    }
    return 0;
}                         
         
void parseAndPrint(FILE *f, char linha[]) {

    char linhaProxima[300];
    fgets(linhaProxima, 300, f);

    if (!feof(f)) {
        parseAndPrint(f, linhaProxima);
    }
    char *values[4];            
    char *index = values[0] = linha;
    int i;
    for (i = 0; i < 3; i++) {      
        index = strstr(index, "|");
        *index = (char) NULL;      
        values[i+1] = ++index;     
    }                              
    char *pic;                
    if (values[3][0] == 'Y') {
        pic = values[0];      
    } else {                  
        pic = "none";         
    }                   
    printf("<tr><td><p align=center>%s</p></td><td><p align=center>%s</p></td><td><p align=center>%s</p></td><td><p align=center>%s</p></td><td><p align=center><img src=/pics/%s.jpg /></p></td></tr>", values[0], values[1], values[2], values[3], pic);

}
