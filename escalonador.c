#include "global.h"


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

	int msgid;
	mensagem msg;

	cria_fila_mensagem();
	do{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, 22, IPC_NOWAIT) < 0) {
		   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{
			printf("PID ESC %d\n", msg.pid);
			kill(msg.pid, SIGUSR1); 
		}
		
	}while(1)
	return 0;
}


/*
* criacao de uma fila de mensagens para leitura se
* ela ainda nao existe
*/

void cria_fila_mensagem(void)
{
	mensagem msg;
    key_fila_msg = 456;

	if (( msgid = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
	     perror("Erro de msgget") ;
	}
	else
	{
		//avisa ao processo shutdown o pid do escalonador
		msg.mtype  = TYPE_SHUTDOWN;
		msg.pid = pid_principal;
		msg.no_source = pid_principal; /*TYPE_NO_eu*/
		msg.no_dest = 0;
		msg.operacao = 0;
		(void) strcpy(msg.mtext,"pid escalonador") ;
		////////
	}
