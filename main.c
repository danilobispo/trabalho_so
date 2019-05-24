
#include "fattree.h"
#include "torus.h"
#include "global.h"


int main(int argc, char const *argv[])
{
	//SINAL NECESSARIO PARA O SHUTDOWN
	signal(SIGUSR1, the_end);
	////--------------



	/*
	* ORDEM DAS FUNCOES PARA A TOPOLOGIA FATTREE 
	*/
	//funcao inicia a topologia e deixa-a pronta pra uso
	inicializa_fattree();
	

	//funcao que manda executar um programa
	//quando ela terminar todos os processos já terao executado e notificado o escalonador
	//e este terá guardado todos os dados na tabela de processos
	aciona_execucao_prog("./a.out", "a");
		


	while(1);
	
	return 0;
}



/*
* funcao que inicializa topologia
* quando a topoliga estiver pronta para ser usada
* retornará 1, caso contrarário 0
*/
void inicializa_fattree(void)
{
	mensagem msg;
	int contador_no_ref = 0;


	/*guarda o pid do escalonador*/
	pid_principal = getpid();


	/*cria a fila de mensagem para comunicacao*/
	cria_fila_mensagem();

	/* 
	* funcao que cria os processos
	*/
	cria_fattree();

}




void aciona_execucao_prog(char *caminho_prog, char *programa)
{
	// printf("VOU EXECUTAR\n");
	if (is_todos_livres())
	{
		// printf("EXECUTANDO...\n");
		ordem_executa_programa(caminho_prog, programa);

		espera_resultado_execucao();
	}
	else
	{
		printf("Não posso executar. Nem todos os processos estão livres!\n");
	}
}





/*
* criacao de uma fila de mensagens para leitura se
* ela ainda nao existe
*/
void cria_fila_mensagem(void)
{
	mensagem msg;
    key_fila_msg = KEY;

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

		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
			perror("[ESCALONADOR]Erro no envio da mensagem") ;
		}
		////////
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


/*
* Essa funcao envia a mensagem com o nome do programa a ser executado
* e marca os processos como ocupados
*/
void ordem_executa_programa(char *caminho_prog, char *programa)
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
	(void) strcpy(msg.mtext,caminho_prog) ;
	(void) strcpy(msg.prog,programa) ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
	}


	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		tab_proc[i].livre = 0;
	}

}

/*
* Essa funcao recebe a resposta dos processos
* e guarda as informações na tabela de processos
*/
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
					// printf("[ESCALONADOR]PROCESSO %d finalizou | %lu -> %lu\n", tab_proc[index].no_ref, msg.time_ini, msg.time_end);
					marca_gerente_livre(msg.pid, msg.time_ini, msg.time_end);

					contador_no_ref++;
				}
			}
		}
	}
}


void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end)
{
	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		if (tab_proc[i].pid == ref)
		{
			tab_proc[i].livre = 1;
			tab_proc[i].time_ini = time_ini;
			tab_proc[i].time_end = time_end;
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


void the_end(int sig)
{
	printf("MATA TODO MUNDO!!\n");

	for (int i = 0; i < N_NOS_FATTREE; ++i)
	{
		kill(tab_proc[i].pid, SIGKILL);
	}

	//no final da execucao do escalonador
	//a fila de msnagem deve ser excluida
	//ATENCAO SOMENTE NO FINAL NAO ANTES
	//SE NAO A TOPOLOGIA NAO FUNCIONARA MAIS
	exclui_fila_mensagem();

	exit(1);
}

/**
 * Realiza a inicializacao da topologia torus e de seus mecanismos de comunicacao caso seja a topologia selecionada
 */

void inicializaTorus(){
// Não implementado aqui, apenas no torus.c, quando eu terminar a topologia trarei pra cá
}