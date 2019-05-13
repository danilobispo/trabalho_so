
// #include <stdio.h> 
// #include <stdlib.h>

#include "fattree.h"
#include "global.h"




int main(int argc, char const *argv[])
{
	// char msg[TAM_MSG] = "ARQUIVO.exe";
	char pid_no0[TAM_MSG];
	int pid_no_fattree;
	int msgid;
	char *path = "fattree.c" ;
	tabela_processos tab_proc[15];
	mensagem msg;




	//dois pipes sao criados para comunicacao
	//entre escalonador e no raiz
    // if(pipe(tubo_esc_no) <0 || pipe(tubo_no_esc) <0) 
    // {
    //     perror("pipe") ;
    //     return -1 ;
    // }



    /*
     * criacao de uma fila de mensagens para leitura se
     * ela ainda nao existe
     */
    key_fila_msg = ftok(path,(key_t)KEY);
	if (( msgid = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
	     perror("Erro de msgget") ;
	     exit(1) ;
	}
   	printf("identificador da fila: %d\n",msgid) ;
   	// printf("esta fila esta associada a chave unica : %#x\n",ftok(path,(key_t)KEY)) ;


	cria_fattree();
	//o escalonador recebe o pid do no raiz do fat tree
	//e armazena
	if (msgrcv(msgid, &msg, sizeof(msg), TYPE_NO_0, 0) == -1) {
	   perror("Erro na recepcao da mensagem") ;
	   exit(1) ;
	}
	printf("PID NO RAIZ %d\n", msg.pid);
	// read(tubo_no_esc[0], pid_no0, sizeof(pid_no0));
	// pid_no_fattree = atoi(pid_no0);
	if (pid_no_fattree != 0)
	{
		tab_proc[0].id_pid = msg.pid;
		tab_proc[0].id_no = 0;
		tab_proc[0].livre = msg.livre;
	}






	

	// write(tubo_esc_no[1], msg, sizeof(msg) + 1);
	// kill(pid_no_fattree, SIGUSR1);

	// printf("enviou kill\n");
	

	return 0;
}