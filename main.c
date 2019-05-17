
// #include <stdio.h> 
// #include <stdlib.h>

#include "fattree.h"
#include "global.h"




int main(int argc, char const *argv[])
{

	mensagem msg;
	int contador_no_ref = 0;



	pid_principal = getpid();

	printf("PAI ESC %d\n", pid_principal);



	cria_fila_mensagem();

	/* 
	* funcao que cria os processos
	*/
	cria_fattree();


	/* 
	* o escalonador recebe os pids dos processos fat tree
	* e armazena na tabela de processos
	* no final eclui a fila de mensagens
	*/
	if (getpid() == pid_principal)
	{
		while(contador_no_ref < 15)
		{
			if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC, IPC_NOWAIT) < 0) {
			   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
			}
			else
			{
				tab_proc[contador_no_ref].pid = msg.pid;
				tab_proc[contador_no_ref].no_ref = contador_no_ref;
				tab_proc[contador_no_ref].livre = msg.livre;

				notifica_filho_ref(msg, contador_no_ref);


				contador_no_ref++;	
			}
		}

		ordem_executa_programa();
		while(1);
		// exclui_fila_mensagem();
	}
	

	return 0;
}



/*
* criacao de uma fila de mensagens para leitura se
* ela ainda nao existe
*/
void cria_fila_mensagem(void)
{
    key_fila_msg = KEY;
	if (( msgid = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
	     perror("Erro de msgget") ;
	}

}


/*
* exclui a fila de mensagem
*/
void exclui_fila_mensagem(void)
{
	if( msgctl(msgid, IPC_RMID, 0) < 0)
	{
		perror("[ESCALONADOR]Erro na exclusao da fila");
	}
	else
	{
		printf("EXCLUI LISTA DE MENSAGEM\n");
	}
}



void notifica_filho_ref(mensagem msg, int no_ref)
{
    /*
	* devolve ao processo a mensagem
	* com sua respectiva referencia
	*/
	msg.mtype  = msg.pid; /*coloca type_no_0 quando quer enviar para o escalonador*/
	// msg.pid = pid;
	msg.no_source = pid_principal; /*TYPE_NO_eu*/
	msg.no_dest = no_ref+1;
	(void) strcpy(msg.mtext,"no ref") ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
	}
}



void ordem_executa_programa(void)
{
	mensagem msg;

    /*
	* enviado para o escalonador dados do processo
	* notificando que já foi criado e está livre
	*/
	msg.mtype  = TYPE_NO_1; //coloca type_no_0 quando quer enviar para o escalonador
	msg.pid = pid_principal;
	msg.no_source = TYPE_ESC; /*TYPE_NO_eu*/
	msg.no_dest = TYPE_ALL;
	msg.livre = false;
	(void) strcpy(msg.mtext,"a.out") ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
	}
	else
	{
		kill(tab_proc[0].pid, SIGUSR1);
	}
}
