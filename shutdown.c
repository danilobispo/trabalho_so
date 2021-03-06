/*
* Grupo:
* Laís Mendes Gonçalves (110033647)
* Guilherme de Oliveira Silva (110012496)
* Danilo José Bispo Galvão (120114852)
* Helena Schubert I. L. Silva (100012311)
*/
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


#define KEY_ESC_SHUT 12497
#define TAM_MSG 100
//quantos bytes tem na struct da mensagem
//100(vetor)+20(vetor)+4(int)+1(bool)+4(int)+4(int)+4(int)+8(lu)+8(lu)
#define TAM_TOTAL_MSG 153



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


int main(int argc, char const *argv[])
{
	// printf("\nentrou na função");
	int msgid;
	mensagem msg;

  	/*  recuperacao do id da fila de mensagens do servidor       */
	if ((msgid = msgget(KEY_ESC_SHUT,0)) == -1) {
	      perror("Erro na criacao da fila do servidor");
	      // exit(1) ;
	}
	else
	{
		// printf("msgid: %d", msgid);
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, 23, IPC_NOWAIT) < 0) {
		   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{
			fflush(stdout);
			// printf("mandei kill|msg.pid:%d", msg.pid);
			fflush(stdout);

			kill(msg.pid, SIGUSR1); 
		}
	}
	return 0;
}
