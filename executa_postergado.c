#include "global.h"
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
//100(vetor)+20(vetor)+4(int)+1(bool)+4(int)+4(int)+4(int)+8(lu)+8(lu)
#define TAM_TOTAL_MSG 153


int main(int argc, char const *argv[])
{
	int msgid;
  int delay;
  char nome_programa[100];
	msg_postergado msg,msg0;

  delay = atoi (argv[1]);
  strcpy (nome_programa,argv[2]);


    	/*  recuperacao do id da fila de mensagens do servidor       */
	if ((msgid = msgget(123,0)) == -1) {
	      perror("Erro na criacao da fila do servidor") ;
	      // exit(1) ;
	}

  while(1){
    if (msgrcv(msgid, &msg0, sizeof(msg0), TYPE_ESC_JOBS, IPC_NOWAIT) < 0) {
		    perror("[postergado]Erro na recepcao da mensagem") ;
		}
    else {
		  msg.mtype  = TYPE_ESC_EXEC;
		  msg.job = msg0.job;
      printf("Job = %d\n", msg0.job);
		  msg.tempo_delay = delay;
		  (void) strcpy(msg.nome_programa,nome_programa);

      if ((msgsnd(msgid, &msg, sizeof(msg), 0)) == -1) {
	       perror("Erro de msgget") ;
	    }

      // //kill (msg0.pid, SIGUSR1);
      break;
    }
  }
	return 0;
}