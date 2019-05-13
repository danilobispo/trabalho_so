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
#include<stdbool.h>


#define TAM_MSG 100
#define TAM_TOTAL_MSG 105
#define TYPE_ESC 100
#define TYPE_NO_0 0




typedef struct
{
	int id_pid;
	int id_no;
	bool livre;
}tabela_processos;


typedef struct
{
    long mtype;
    int pid;
    char mtext[TAM_MSG];
    bool livre;
}mensagem;



int key_fila_msg ;  /* ID da fila de mensagens */
int tubo_esc_no[2];
int tubo_no_esc[2];


#endif