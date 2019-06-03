/*
* Grupo:
* Laís Mendes Gonçalves (110033647)
* Guilherme de Oliveira Silva (110012496)
* Danilo José Bispo Galvão (120114852)
* Helena Schubert I. L. Silva (100012311)
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H



#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>


#include <time.h>


#define TAM_MSG 100
//quantos bytes tem na struct da mensagem
//100(vetor)+20(vetor)+4(int)+1(bool)+4(int)+4(int)+4(int)+8(lu)+8(lu)
#define TAM_TOTAL_MSG 153
#define KEY_FAT_TREE 33647
#define KEY_TORUS 14852
#define N_NOS_FATTREE 15
#define N_NOS_CUBO_TORUS 16



enum types_mensagens { TYPE_NO_1 = 1, TYPE_NO_2, TYPE_NO_3, TYPE_NO_4, TYPE_NO_5, TYPE_NO_6, TYPE_NO_7,
TYPE_NO_8, TYPE_NO_9, TYPE_NO_10, TYPE_NO_11, TYPE_NO_12, TYPE_NO_13, TYPE_NO_14, TYPE_NO_15, TYPE_NO_16, TYPE_ESC, TYPE_ALL,
TYPE_INI, TYPE_EXEC, TYPE_FIN, TYPE_STOP, TYPE_SHUTDOWN, TYPE_ESC_EXEC, TYPE_ESC_JOBS} types_msg;


typedef struct
{
	int pid;
	int no_ref;
    unsigned long time_ini;
    unsigned long time_end;
	bool livre;
}tabela_processos;

typedef struct
{
	int jobs;
	char nome_programa[100];
    char nome_programa_sem_path[100];
    int tempo_delay;
	time_t tempo_futuro;
    time_t inicio;
    bool executado;
    unsigned long makespan;
}tabela_job_processos;

typedef struct
{
    long mtype;
    int pid;
    int no_source;
    int no_dest;
    int operacao;
    unsigned long time_ini;
    unsigned long time_end;
    bool livre;
    char mtext[TAM_MSG];
    char prog[20];
}mensagem;


typedef struct
{
    long mtype;
    int pid;
    int job;
    int tempo_delay;
    char nome_programa[100];
}msg_postergado;


int key_fila_msg;
tabela_processos tab_proc[16];
int msgid;
int pid_principal;
tabela_job_processos tab_job[100];



//funcoes principais para comunicacao com a topologia FATTREE
void inicializa_fattree(void);
// void aciona_execucao_prog(char *caminho_prog, char *programa);
//void exclui_fila_mensagem(void);
void the_end(int sig);

//função auxiliares para comunicação com o executa postergado
void cria_fila_mensagem_postergado(void);

//funcoes auxiliares para comunicacao com a topologia FATTREE
void cria_fila_mensagem(void);
// void ordem_executa_programa(char *caminho_prog, char *programa);
// void espera_resultado_execucao(void);
// void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end);
// int is_todos_livres(void);
int search_proc(int ref, int option);
//-------------------------------


//funcoes principais para comunicacao com a topologia TORUS
//TODO: Adequar o makefile para poder receber essa função e o torus.c/.h na criação da topologia
void inicializaTorus();



#endif
