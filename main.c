
// #include <stdio.h> 
// #include <stdlib.h>

#include "fattree.h"
#include "global.h"




int main(int argc, char const *argv[])
{

	mensagem msg;
	int contador_no_ref = 0;
	int fattree_pronto = 0;



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
		while(contador_no_ref < N_NOS_FATTREE)
		{
			if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC, IPC_NOWAIT) < 0) {
			   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
			}
			else
			{
				if (msg.operacao == TYPE_INI)
				{
					tab_proc[contador_no_ref].pid = msg.pid;
					tab_proc[contador_no_ref].no_ref = contador_no_ref+1;
					tab_proc[contador_no_ref].livre = msg.livre;

					notifica_filho_ref(msg, contador_no_ref);

					// printf("NO REF = %d | %d\n", tab_proc[contador_no_ref].no_ref, tab_proc[contador_no_ref].pid);

					contador_no_ref++;	
				}
			}
		}

		//verificando se a topologia ja esta pronta para ser usada
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC, 0) < 0) {
		   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{
			if (strcmp (msg.mtext, "topologia feita!") == 0)
			{
				fattree_pronto = 1;
			}
		}





		if (fattree_pronto)
		{
			printf("VOU EXECUTAR\n");
			if (is_todos_livres())
			{
				printf("EXECUTANDO...\n");
				ordem_executa_programa();

				espera_resultado_execucao();
			}
		}
		

		while(1);
		// exclui_fila_mensagem();
	}
	
	while(1);
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
	msg.mtype  = msg.pid;
	// msg.pid = pid;
	msg.no_source = pid_principal; /*TYPE_NO_eu*/
	msg.no_dest = no_ref+1;
	msg.operacao = TYPE_INI;
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
	msg.operacao = TYPE_EXEC;
	(void) strcpy(msg.mtext,"a") ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
	}
	// else
	// {
	// 	int index = search_proc_index(1);
	// 	if (index == -1)
	// 	{
	// 		printf("[ESCALONADOR] NO NAO ENCONTRADO\n");
	// 	}
	// 	else
	// 	{
	// 		kill(tab_proc[index].pid, SIGUSR1);
	// 		// kill(0, SIGUSR1);
	// 		printf("[ESCALONADOR]MANDEI MENSAGEM %d | %d\n", tab_proc[index].no_ref, tab_proc[index].pid);
	// 	}
	// }


	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		tab_proc[i].livre = 0;
	}

}

void espera_resultado_execucao(void)
{
	int contador_no_ref = 0;
	int index;
	mensagem msg;

	while(contador_no_ref < N_NOS_FATTREE)
	{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC, IPC_NOWAIT) < 0) {
		   // perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{
			if (msg.operacao == TYPE_FIN)
			{
				index = search_proc(msg.pid, 2);

				if (index != -1)
				{
					printf("[ESCALONADOR]PROCESSO %d finalizou\n", tab_proc[index].no_ref);
					marca_gerente_livre(msg.pid);

					contador_no_ref++;
				}
			}
		}
	}

	printf("------------>FINALIZE\n");
}


void marca_gerente_livre(int ref)
{
	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		if (tab_proc[i].pid == ref)
		{
			tab_proc[i].livre = 1;
		}
	}
}


int is_todos_livres(void)
{
	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		if (tab_proc[i].livre == 0)
		{
			return 0;
		}
	}

	return 1;
}

/*
* option = 1 -> no_ref
* option = 2 -> pid
*/
int search_proc(int ref, int option)
{
	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		if (option == 1)
		{
			if (tab_proc[i].no_ref == ref)
			{
				return i;
			}
		}
		else if(option == 2)
		{
			if (tab_proc[i].pid == ref)
			{
				return i;
			}

		}
	}

	return -1;
}
