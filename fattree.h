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
#define KEY 123



typedef struct
{
	int id_pid;
	int id_no;
}arvore_processos;




// int cont_no = 0;

// char *path = "fattree.c" ;
pid_t pid; 
arvore_processos arvore[N_NOS];
char msg_recebida[TAM_MSG];


void print_arvore(arvore_processos arvore[]);
void cria_fattree(void);
void mensagem_no_raiz(int signum);




#endif