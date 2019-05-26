#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(){

	int i=0;
	int j=0;
	int m=0;
	int fork0, fork1, fork2, fork3;
	int forkcircularvertical;
	int forkcircularhorizontal;
	int n=4;
    
    enum types_mensagens { TYPE_NO_1 = 1, TYPE_NO_2, TYPE_NO_3, TYPE_NO_4, TYPE_NO_5, TYPE_NO_6, TYPE_NO_7,
        TYPE_NO_8, TYPE_NO_9, TYPE_NO_10, TYPE_NO_11, TYPE_NO_12, TYPE_NO_13, TYPE_NO_14, TYPE_NO_15, TYPE_ESC, TYPE_ALL,
        TYPE_INI, TYPE_EXEC, TYPE_FIN, TYPE_STOP, TYPE_SHUTDOWN} types_msg;
	
	// Pode vir a ser ?til, pois a fat tree tem 15 eu acho, tem que ser um par?metro passado quando
	// a topologia for criada, no caso aqui o torus tem 16
	int n_nos_topologia = 16;
	
	typedef struct {
		int no_id; // N?mero do n?
		int vizinhos[4]; // N?mero m?ximo de vizinhos
		//0 para falso, 1 para verdadeiro
		int ocupado; // Verifica se o n? j? est? transmitindo uma mensagem ou n?o
		char* mensagem;
	} NO_TORUS;
	
	// Tornei static s? pra n?o ficar brincando com ponteiro em fun??es simples e dificultar leitura do c?digo
	static NO_TORUS no_torus[16]; // N?s da topologia Torus
	
	
	/**
	Inicializa os n?s do torus
	**/
	void cria_nos_torus(){
		for (i;i<16;i++){
			no_torus[i].no_id = i;
			no_torus[i].ocupado = 0; // A princ?pio, todos os n?s est?o livres
			for(j=0; j<4; j++){
				no_torus[i].vizinhos[j] = -1; // Vizinho inv?lido, quando a inser??o for realizada
				// esse valor ser? necess?rio para checar se eu n?o estou sobrescrevendo nada
			}
		}
	}
	
	/**
	Verifica se vizinho a ser adicionado j? n?o existe no vetor de vizinhos daquele n?
	**/
	int is_vizinho_duplicado(int no_atual, int vizinho){
		int n;
		int i;
		n = sizeof(no_torus[no_atual].vizinhos);
		for(i=0; i<4; i++){
			if(no_torus[no_atual].vizinhos[i] == vizinho){
				return 1; // Se for duplicado retorna true;
			}
		}
		return 0; // Else retorna false;
	}
	
	/**
	Adiciona vizinho diretamente
	**/
	void adicionar_vizinho(int no_atual, int vizinho){
		int j=0;
		int adicionou = 0;
		while(!adicionou && j<4){
			if(no_torus[no_atual].vizinhos[j] == -1){ // Se a posi??o j estiver vazia...
				no_torus[no_atual].vizinhos[j] = vizinho; // Adiciona novo vizinho
				adicionou = 1;
			}
			j++;
		}
		if(adicionou == 0) {
//			printf("Erro, no ja possui o maximo de vizinhos! No: %d\n", no_atual);
//			printf("Vizinho que n?o foi adicionado: %d\n", vizinho);
		}
	}
	
	/**
	Informa vizinho que n? x ? vizinho, vizinho ent?o adiciona o dado n?, caso n?o seja duplicado
	**/
	void informar_vizinho(int no_atual, int no_id_vizinho){
//	printf("Ei, no %d!\n%d eh seu vizinho!\n", no_atual, no_id_vizinho);
	//TODO: N? atual agora recebe na sua lista de vizinhos o n? vizinho
		if(!is_vizinho_duplicado(no_atual, no_id_vizinho)){
			adicionar_vizinho(no_atual, no_id_vizinho);
		}
	}
	
	/**
	Imprime os n?s do torus, s? serve pra debug atualmente
	**/
	void imprime_nos_torus(){
		int i=0;
		int j=0;
		char ocupado[4];
		
		for(i; i < 16; i++){
//			printf("No atual: %d\n", no_torus[i].no_id);
			no_torus[i].ocupado ? stpcpy(ocupado,"Sim") : strcpy(ocupado,"Nao");
//			printf("Ocupado? %s\n", ocupado);
			for(j=0; j<4 ; j++){ //Imprime n?s vizinhos
//				printf("Nos vizinhos: %d\n", no_torus[i].vizinhos[j]);
			}
		}
	}
	
	int isVizinho(int no_ref, int no){
	    for (i=0; i < 4 ; i++) {
            if (no_torus[no_ref].vizinhos[i] == no) {
                printf("eh vizinho\n");
                return 1;
            }
        }
	    printf("Nao, no_ref %d nao possui o vizinho %d\n", no_ref, no);
	    return 0;
	}
	
	int calculaCaminho(int no_dest, int tipo_msg){
	    int type_all = no_dest == TYPE_ALL ? 1:0;
	    int type_escalonador  = no_dest == TYPE_ESC ? 1:0;
	    // Valores absolutos para no de origem e no destino
	    int destino = no_dest - 1; // Valores absolutos
        int no_ref = tipo_msg - 1; // Valores absolutos
        
        printf("Destino: %d\nno_ref:%d\n", destino, no_ref);
        
	    int i;
	    unsigned int menorvalor;
	    int melhorVizinho;
	    
        // Primeiro caso: Quero mandar para todos
        
            // 0 manda para 1, 4
            //msgsnd()
            //msgsnd()
            // 1 manda para 2, 5
            //msgsnd()
            //msgsnd()
            // 2 manda para 3, 6
            //msgsnd()
            //msgsnd()
            // 3 manda para 7
            //msgsnd()
            // 4 manda para 8
            //msgsnd()
            // 5 manda para 9
            //msgsnd()
            // 6 manda para 10
            //msgsnd()
            // 7 manda para 11
            //msgsnd()
            // 8 manda para 12
            //msgsnd()
            // 9 manda para 13
            //msgsnd()
            // 10 manda para 14
            //msgsnd()
            // 11 manda para 15
            //msgsnd()
	    
	    // Segundo caso, calcula nó específico:
	    if(!type_all && !type_escalonador) {
        
            // Distância é nó de destino - o nó de origem
            // Se o nó destino for o escalonador, isso será negativo...
            // TODO: Fazer o caso de nó voltando para o nó 0 e para o escalonador
        
            int dist = abs(destino - no_ref);
            printf("dist: %d\n", dist);
        
        
            // Caso especifico: distancia entre os nos eh 1, mas o no nao eh vizinho do outro
            // ex 8 e 7 ou 7 e 8
            
            if (dist == 1 && !isVizinho(no_ref, destino) ) {
                printf("Nao sao vizinhos\n");
                if ((destino - no_ref) > 0) {
                        printf(" > 0\n");
                        if (isVizinho(no_ref + 4, destino)) {
                            printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref, no_ref + 4);
                            printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref + 4, destino);
                        }
                } else if ((destino - no_ref) < 0) {
                    printf(" < 0\n");
                    if (isVizinho(no_ref - 4, destino)) {
                        printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref, no_ref - 4);
                        printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref - 4, destino);
                    }
                }
            }
        
            for (i = 0; i < 4; i++) {
                printf("vizinhos: %d\n", no_torus[no_ref].vizinhos[i]);
            }
            printf("destino: %d\n", destino);
            menorvalor = destino - no_torus[no_ref].vizinhos[0];
            // Calculo o modulo, que eh a distância absoluta entre os nos, independente de ser menor ou maior que 0
            menorvalor = abs(menorvalor);
            printf("menorvalor: %d \n", menorvalor);
            melhorVizinho = no_torus[no_ref].vizinhos[0];
            for (i = 1; i < 4; i++) {
                if (menorvalor > abs(destino - no_torus[no_ref].vizinhos[i])) {
                    menorvalor = abs(destino - no_torus[no_ref].vizinhos[i]);
                    printf("menorvalor: %d \n", menorvalor);
                    melhorVizinho = no_torus[no_ref].vizinhos[i];
                }
            }
            printf("Melhor Vizinho: %d\n", melhorVizinho);
            printf("Manda mensagem(origem: %d, destino:%d)", no_ref, melhorVizinho);
        }
        
        return melhorVizinho+1;
	    // Terceiro caso: quero mandar de volta para o escalonador
	}
	
	
	cria_nos_torus();
	for(i=0;i<n;i++){
//		printf("Fila %d\n", i);
		for(j=0;j<n;j++){
			fork0 = m - 1; // Vizinho da esquerda
			fork1 = m + 1; // Vizinho da direita
			fork2 = m + 4; // Vizinho abaixo
			fork3 = m - 4; // Vizinho do topo
//			printf("Nó atual: %d\t", m);
			if(j != n-1){
//				printf("Vizinho direita: %d(%d+1)\n", fork1, m);
				//adicionar_vizinho(m, fork1);
				informar_vizinho(fork1, m);
			}
			if(i!=n-1){
//				printf("Vizinho abaixo: %d(%d+4)\n", fork2, m);
				adicionar_vizinho(m, fork2);
				informar_vizinho(fork2, m);
			}
			
			if(i==0){ // Primeira fila
				forkcircularvertical = m+12;
//				printf("Vizinho Circular Vertical: %d(%d+12)\n", forkcircularvertical, m);
				adicionar_vizinho(m, forkcircularvertical);
				informar_vizinho(forkcircularvertical, m);
			}
			if(j==0){
				forkcircularhorizontal = m+3;
//				printf("Vizinho Circular Horizontal: %d(%d+3)\n", forkcircularhorizontal, m);
				adicionar_vizinho(m, forkcircularhorizontal);
				informar_vizinho(forkcircularhorizontal, m);
			}
			
			if((fork0) >= 0 ){
//				printf("Vizinho esquerda: %d(%d-1)\n", fork0, m);
				informar_vizinho(fork0, m);
			}
			
			if((fork3) >= 0){
//				printf("Vizinho topo: %d(%d-4)\n", fork3, m);
				informar_vizinho(fork3, m);
				
			}
			m++;
		}
	}
	//imprime_nos_torus();
	
	// Caso positivo: do nó 0 até o nó 9
	int valor = TYPE_NO_1;
//	i = 0;
//
//	while(valor != TYPE_NO_10){
//
//	    printf("\niteracao %d\n", i);
//        valor = calculaCaminho(TYPE_NO_10, valor); // nó 4 é o atual, para no nó 8
//        printf("valor: %d\n", valor);
//        i++;
//    }

	
//	 Caso negativo: do no 9 ate o no 0
//	valor = TYPE_NO_10;
//    i = 0;
//    while(valor != TYPE_NO_1){
//        printf("\niteracao %d\n", i);
//        valor = calculaCaminho(TYPE_NO_1, valor);
//        printf("valor: %d\n", valor);
//        i++;
//    }
    
    valor = TYPE_NO_13;
    i=0;
    while(valor != TYPE_NO_8){
        printf("\niteracao %d\n", i);
        valor = calculaCaminho(TYPE_NO_8, valor);
        printf("valor: %d\n", valor);
        i++;
        if(i == 4){
            return 0;
        }
    }
	
	// Segundo caso positivo: 0 até o 10
//    valor = TYPE_NO_3;
//    while(valor != TYPE_NO_11){
//        valor = calculaCaminho(TYPE_NO_11, valor);
//    }
	
//	int valornegativo = TYPE_NO_12; // Nó 11
//    while(valornegativo != TYPE_NO_5){
//        valornegativo = calculaCaminho(TYPE_NO_5, valornegativo); // nó 4 é o atual, para no nó 8
//    }
	   
    
	return 0;
}