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
#define N_NOS 4







// int cont_no = 0;


int pid;
int no_ref;
int msgid;


void cria_fattree(void);
// void transmite_msg(mensagem);
// void roteamento_msg(mensagem);
void registra_processo(void);
void guarda_ref(void);




#endif