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




#define TAM_MSG 100
//quantos bytes tem na struct da mensagem
//100(vetor)+4(int)+1(bool)+4(int)+4(int)
#define TAM_TOTAL_MSG 113
#define KEY 123



enum types_mensagens { TYPE_NO_1 = 1, TYPE_NO_2, TYPE_NO_3, TYPE_NO_4, TYPE_NO_5, TYPE_NO_6, TYPE_NO_7,
TYPE_NO_8, TYPE_NO_9, TYPE_NO_10, TYPE_NO_11, TYPE_NO_12, TYPE_NO_13, TYPE_NO_14, TYPE_NO_15, TYPE_ESC, TYPE_ALL} types_msg;


typedef struct
{
	int pid;
	int no_ref;
	bool livre;
}tabela_processos;


typedef struct
{
    long mtype;
    int pid;
    int no_source;
    int no_dest;
    bool livre;
    char mtext[TAM_MSG];
}mensagem;




int key_fila_msg ;  /* ID da fila de mensagens */
tabela_processos tab_proc[15];
int msgid;
int pid_principal;




void cria_fila_mensagem(void);
void exclui_fila_mensagem(void);
void notifica_filho_ref(mensagem msg, int no_ref);
void ordem_executa_programa(void);


#endif