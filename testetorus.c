#include <stdio.h>
#include <string.h>
int main(){

	int i=0;
	int j=0;
	int m=0;
	int fork0, fork1, fork2, fork3;
	int forkcircularvertical;
	int forkcircularhorizontal;
	int n=4;
	
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
			printf("Erro, no ja possui o maximo de vizinhos! No: %d\n", no_atual);
			printf("Vizinho que n?o foi adicionado: %d\n", vizinho);
		}
	}
	
	/**
	Informa vizinho que n? x ? vizinho, vizinho ent?o adiciona o dado n?, caso n?o seja duplicado
	**/
	void informar_vizinho(int no_atual, int no_id_vizinho){
	printf("Ei, no %d!\n%d eh seu vizinho!\n", no_atual, no_id_vizinho);
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
			printf("No atual: %d\n", no_torus[i].no_id);
			no_torus[i].ocupado ? stpcpy(ocupado,"Sim") : strcpy(ocupado,"Nao");
			printf("Ocupado? %s\n", ocupado);
			for(j=0; j<4 ; j++){ //Imprime n?s vizinhos
				printf("Nos vizinhos: %d\n", no_torus[i].vizinhos[j]);
			}
		}
	}	
	
	
	cria_nos_torus();
	for(i=0;i<n;i++){
		printf("Fila %d\n", i);
		for(j=0;j<n;j++){
			fork0 = m - 1; // Vizinho da esquerda
			fork1 = m + 1; // Vizinho da direita
			fork2 = m + 4; // Vizinho abaixo
			fork3 = m - 4; // Vizinho do topo
			printf("Nó atual: %d\t", m);
			if(j != n-1){
				printf("Vizinho direita: %d(%d+1)\n", fork1, m);
				//adicionar_vizinho(m, fork1);
				informar_vizinho(fork1, m);
			}
			if(i!=n-1){
				printf("Vizinho abaixo: %d(%d+4)\n", fork2, m); 
				adicionar_vizinho(m, fork2);
				informar_vizinho(fork2, m);
			}
			
			if(i==0){ // Primeira fila
				forkcircularvertical = m+12;
				printf("Vizinho Circular Vertical: %d(%d+12)\n", forkcircularvertical, m);
				adicionar_vizinho(m, forkcircularvertical);
				informar_vizinho(forkcircularvertical, m);
			}
			if(j==0){
				forkcircularhorizontal = m+3;
				printf("Vizinho Circular Horizontal: %d(%d+3)\n", forkcircularhorizontal, m);
				adicionar_vizinho(m, forkcircularhorizontal);
				informar_vizinho(forkcircularhorizontal, m);
			}
			
			if((fork0) >= 0 ){
				printf("Vizinho esquerda: %d(%d-1)\n", fork0, m);
				informar_vizinho(fork0, m);
			}
			
			if((fork3) >= 0){
				printf("Vizinho topo: %d(%d-4)\n", fork3, m);
				informar_vizinho(fork3, m);
				
			}
			m++;
		}
	}
	imprime_nos_torus();
	return 0;
}