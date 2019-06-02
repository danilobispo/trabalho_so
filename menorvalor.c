#include <stdio.h>
#include <string.h>


int main() {
    
    int vizinhos[4] = {1,12,4,3};
    unsigned int menorvalor = 17;
    int destino = 11;
    int origem = 0;
    int melhorcaminho = 0;
    int i = 1;
    menorvalor = destino - vizinhos[0];
    printf("menorvalor: %d\n", menorvalor);
    for(i; i<4; i++) {
        unsigned int temp = menorvalor;
        if(destino - vizinhos[i] < menorvalor) {
            menorvalor = destino - vizinhos[i];
            printf("menorvalor: %d\n", menorvalor);
            melhorcaminho = vizinhos[i];
        }
    }
    printf("Menor valor: %d\n", menorvalor);
    printf("Melhor caminho: %d\n", melhorcaminho);
    return 0;
}