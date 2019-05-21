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
#include <stdbool.h>
#include <string.h>


#include <time.h>




#define TAM_MSG 100
//quantos bytes tem na struct da mensagem
//100(vetor)+20(vetor)+4(int)+1(bool)+4(int)+4(int)+4(int)+8(lu)+8(lu)
#define TAM_TOTAL_MSG 153
#define KEY 123
#define N_NOS_FATTREE 15



enum types_mensagens { TYPE_NO_1 = 1, TYPE_NO_2, TYPE_NO_3, TYPE_NO_4, TYPE_NO_5, TYPE_NO_6, TYPE_NO_7,
TYPE_NO_8, TYPE_NO_9, TYPE_NO_10, TYPE_NO_11, TYPE_NO_12, TYPE_NO_13, TYPE_NO_14, TYPE_NO_15, TYPE_ESC, TYPE_ALL,
TYPE_INI, TYPE_EXEC, TYPE_FIN, TYPE_STOP} types_msg;


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




int key_fila_msg ;  /* ID da fila de mensagens */
tabela_processos tab_proc[15];
int msgid;
int pid_principal;




//funcoes principais para comunicacao com a topologia FATTREE
int inicializa_fattree(void);
void aciona_execucao_prog(char *caminho_prog, char *programa);
void exclui_fila_mensagem(void);

//funcoes auxiliares para comunicacao com a topologia FATTREE
void cria_fila_mensagem(void);
void notifica_filho_ref(mensagem msg, int no_ref);
void ordem_executa_programa(char *caminho_prog, char *programa);
void espera_resultado_execucao(void);
void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end);
int is_todos_livres(void);
int search_proc(int ref, int option);
//-------------------------------





#endif