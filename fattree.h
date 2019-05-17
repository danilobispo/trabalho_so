#ifndef _FATTREE_H
#define _FATTREE_H


#include <signal.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "global.h"

//2^n-1 = numeros de processos criados por n forks
#define N_FORKS 4
#define N_NOS 15







// int cont_no = 0;


int pid;
int no_ref;
int msgid;
int vetor_folhas[8] = {4, 5, 7, 8, 14, 15, 12, 11};
int vetor_nos[14];


void cria_fattree(void);
void executa_programa(int signum);
void roteamento_msg(mensagem);
void registra_processo(void);
void guarda_ref(void);
void acorda_filhos(void);
void fim_programa(int signum);
int is_folha(void);




#endif