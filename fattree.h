#ifndef _FATTREE_H
#define _FATTREE_H


#include <signal.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "global.h"


#define N_NOS 15



typedef struct
{
	int pid;
	int no_ref;
}nos;





int pid;
int no_ref;
int msgid;
nos nos_vetor[N_NOS];

time_t timer_inicio;
time_t timer_final;
struct tm *horarioLocal_inicio;
struct tm *horarioLocal_final;
clock_t tempos[2];


void cria_fattree(void);
void executa_programa(void);
void roteamento_msg(mensagem);
void fim_programa(unsigned long time_ini, unsigned long time_end);
int is_folha(void);
void trata_mensagem(char *caminho_prog, char *programa);




#endif