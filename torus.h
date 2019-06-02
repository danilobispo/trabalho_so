//
// Created by danilo on 24/05/19.
//

#ifndef TRABALHO_SO_TORUS_H
#define TRABALHO_SO_TORUS_H

#endif //TRABALHO_SO_TORUS_H
#define n_nos_topologia 16
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "global.h"


// Pode vir a ser util, pois a fat tree tem 15 eu acho, tem que ser um parametro passado quando
// a topologia for criada, no caso aqui o torus tem 16


/*typedef struct
{
    int pid;
    int no_ref;
}nos;*/

typedef struct {
    int pid;    // Pid do processo desse no
    int noId; // Numero do no, ou seja, a id do no que determinamos de 0 a 15
    int vizinhos[4]; // Numero maximo de vizinhos
    int livre; // Verifica se o no ja esta executando algum processo
    // 0 para falso, 1 para verdadeiro
} NO_TORUS;

static NO_TORUS noTorus[n_nos_topologia];

int pid;
int no_ref;
int contador_rcv;
int msgid;

time_t timer_inicio;
time_t timer_final;
struct tm *horarioLocal_inicio;
struct tm *horarioLocal_final;

clock_t tempos[2];

// Funcoes de torus.c
void criaNosTorus();
int isVizinhoDuplicado(int noAtual, int vizinho);
void adicionarVizinho(int noAtual, int vizinho);
void informarVizinho(int no_atual, int noIdVizinho);
void imprimeNosTorus();
void criaProcessosTorus();
int isVizinho(int no_ref, int no);
int calculaCaminho(mensagem msg, int no_dest, int no_orig);
int isNosLivresTorus();
void executaProgramaNosTorus();


//Remover depois de juntar com a main
void inicializaTorus();
void cria_fila_mensagem();
void trata_mensagem(char *caminho_prog, char *programa);
void fim_programa(unsigned long time_ini, unsigned long time_end);
void aciona_execucao_prog(char *caminho_prog, char *programa);
void ordem_executa_programa(char *caminho_prog, char *programa);

int search_proc(int ref, int option);
void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end);


