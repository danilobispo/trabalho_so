//
// Created by oem on 24/05/19.
//

#include "torus.h"
#include "global.h"

int main(){

    printf("Topologia Torus selecionada\n");
    inicializaTorus();
    
}

/**
	Verifica se vizinho a ser adicionado ja nao existe no vetor de vizinhos daquele no
	**/
int isVizinhoDuplicado(int noAtual, int vizinho){
    int n;
    int i;
    n = sizeof(noTorus[noAtual].vizinhos);
    for(i=0; i<4; i++){
        if(noTorus[noAtual].vizinhos[i] == vizinho){
            return 1; // Se for duplicado retorna true;
        }
    }
    return 0; // Else retorna false;
}

/**
Adiciona vizinho diretamente
**/
void adicionarVizinho(int noAtual, int vizinho){
    int j=0;
    int adicionou = 0;
    while(!adicionou && j<4){
        if(noTorus[noAtual].vizinhos[j] == -1){ // Se a posicao j estiver vazia...
            noTorus[noAtual].vizinhos[j] = vizinho; // Adiciona novo vizinho
            adicionou = 1;
        }
        j++;
    }
    if(adicionou == 0) {
//        DEBUG
//        printf("Erro, no ja possui o maximo de vizinhos! No: %d\n", noAtual);
//        printf("Vizinho que nao foi adicionado: %d\n", vizinho);
    }
}

/**
Informa vizinho que n? x ? vizinho, vizinho ent?o adiciona o dado n?, caso n?o seja duplicado
**/
void informarVizinho(int noAtual, int noIdVizinho){
    //printf("Ei, no %d!\n%d eh seu vizinho!\n", noAtual, noIdVizinho);
    //TODO: N? atual agora recebe na sua lista de vizinhos o n? vizinho
    if(!isVizinhoDuplicado(noAtual, noIdVizinho)){
        adicionarVizinho(noAtual, noIdVizinho);
    }
}

/**
Imprime os n?s do torus, s? serve pra debug atualmente
**/
void imprimeNosTorus(){
    int i=0;
    int j=0;
    char ocupado[4];

    for(i; i < 16; i++){
        printf("No atual: %d\n", noTorus[i].noId);
        noTorus[i].ocupado ? stpcpy(ocupado,"Sim") : strcpy(ocupado,"Nao");
        printf("Ocupado? %s\n", ocupado);
        for(j=0; j<4 ; j++){ //Imprime n?s vizinhos
            printf("Nos vizinhos: %d\n", noTorus[i].vizinhos[j]);
        }
    }
}

/**
	Inicializa os n?s do torus
	**/
void criaNosTorus() {
    int i = 0;
    int j = 0;
    int m = 0;
    int n = 4;
    int fork0, fork1, fork2, fork3;
    int forkcircularvertical;
    int forkcircularhorizontal;


    // Visão abstrata dos processos, nenhum fork foi realizado por ora
    for (i; i < n_nos_topologia; i++){
        noTorus[i].noId = i;
        noTorus[i].ocupado = 0; // A principio, todos os nos estao livres
        for(j=0; j<4; j++){
            noTorus[i].vizinhos[j] = -1; // Vizinho invalido, quando a insercao for realizada
            // esse valor sera necessario para checar se eu nao estou sobrescrevendo nada
        }
    }

    // Zerando as variáveis para um novo loop:
    i=0;
    j=0;

    // Aqui adicionamos os vizinhos a todos os nós
    for(i=0;i<n;i++){
//        DEBUG
//        printf("Fila %d\n", i);
        for(j=0;j<n;j++){
            fork0 = m - 1; // Vizinho da esquerda
            fork1 = m + 1; // Vizinho da direita
            fork2 = m + 4; // Vizinho abaixo
            fork3 = m - 4; // Vizinho do topo
//            DEBUG
//            printf("Nó atual: %d\t", m);
            if(j != n-1){
//                printf("Vizinho direita: %d(%d+1)\n", fork1, m);
                //adicionarVizinho(m, fork1);
                informarVizinho(fork1, m);
            }
            if(i!=n-1){
//                DEBUG
//                printf("Vizinho abaixo: %d(%d+4)\n", fork2, m);
                adicionarVizinho(m, fork2);
                informarVizinho(fork2, m);
            }

            if(i==0){ // Primeira fila
                forkcircularvertical = m+12;
//                DEBUG
//                printf("Vizinho Circular Vertical: %d(%d+12)\n", forkcircularvertical, m);
                adicionarVizinho(m, forkcircularvertical);
                informarVizinho(forkcircularvertical, m);
            }
            if(j==0){
                forkcircularhorizontal = m+3;
//                DEBUG
//                printf("Vizinho Circular Horizontal: %d(%d+3)\n", forkcircularhorizontal, m);
                adicionarVizinho(m, forkcircularhorizontal);
                informarVizinho(forkcircularhorizontal, m);
            }

            if((fork0) >= 0 ){
//                DEBUG
//                printf("Vizinho esquerda: %d(%d-1)\n", fork0, m);
                informarVizinho(fork0, m);
            }

            if((fork3) >= 0){
//                DEBUG
//                printf("Vizinho topo: %d(%d-4)\n", fork3, m);
                informarVizinho(fork3, m);

            }
            m++;
        }
    }
    // End adicionar vizinhos

}


void criaProcessosTorus(){
    int i = 0;
    int pid_fork;
    int contador_no_ref = 0;

    // pid do escalonador
    pid_principal = getpid();


    /*criando os processos*/
    for (i; i < n_nos_topologia; i++)
    {
        if (getpid() == pid_principal)
        {
            no_ref = contador_no_ref+1;

            pid_fork = fork();

            if (pid_fork > 0)
            {
                // Aqui os processos sao inseridos na tabela de processos, localizada
                // na global.h
                tab_proc[contador_no_ref].pid = pid_fork;
                tab_proc[contador_no_ref].no_ref = contador_no_ref+1;
                tab_proc[contador_no_ref].livre = 1;

                contador_no_ref++;
            }
        }
        else
        {
            break;
        }
    }

    if (getpid() == pid_principal)
    {
        for(i=0; i<n_nos_topologia; i++){
//            DEBUG
            printf("tabproc[%d]\npid: %d\nno_ref:%d\n", i, tab_proc[i].pid, tab_proc[i].no_ref);
        }
    }
}

void inicializaTorus(){
//  Cria estrutura com a mensagem a ser repassada entre os nós
    mensagem msg;
    int i = 0;
    
    // Aqui os nos e seus vizinhos sao criados como estruturas de dados
    // sem forks ainda
    criaNosTorus();
    
    /*guarda o pid do escalonador*/
    pid_principal = getpid();
    
    /*cria a fila de mensagem para comunicacao*/
    cria_fila_mensagem();
    
    /*
    * funcao que cria os processos
    */
//    TODO: Arrumar isso aqui pra funcionar direito
//    criaProcessosTorus();

}

//Copia de cria_fila_mensagem
//TODO: Remover apos terminar topologia e juntar com a main
void cria_fila_mensagem(void)
{
    mensagem msg;
    key_fila_msg = KEY;
    
    if (( msgid = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
        perror("Erro de msgget") ;
    }
    else
    {
        //avisa ao processo shutdown o pid do escalonador
        msg.mtype  = TYPE_SHUTDOWN;
        msg.pid = pid_principal;
        msg.no_source = pid_principal; /*TYPE_NO_eu*/
        msg.no_dest = 0;
        msg.operacao = 0;
        (void) strcpy(msg.mtext,"pid escalonador") ;
        
        if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
            perror("[ESCALONADOR]Erro no envio da mensagem") ;
        }
        ////////
    }
    
}








