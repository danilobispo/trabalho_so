/*
* Grupo:
* Laís Mendes Gonçalves (110033647)
* Guilherme de Oliveira Silva (110012496)
* Danilo José Bispo Galvão (120114852)
* Helena Schubert I. L. Silva (100012311)
*/
#include "fattree.h"



/*
* Funcao para criar topologia Fat Tree
* ela faz o fork 15 vezes para criar 15 processos
* todos filhos do escalonador
*/
void cria_fattree()
{
	int cont_no = 0; 
	int pid_fork;
	int contador_no_ref = 0;
  	

	//guarda o pid do processo do escalonador
	//usado em referencias futuras
  	pid_principal = getpid();


   	/*criando os processos*/
	for (cont_no = 0; cont_no < N_NOS; cont_no++)
	{
		//se for o escalonador ele faz o fork
		if (getpid() == pid_principal)
		{
			//o escalonador incrementa o contador de referencias
			//assim quando o filho for criado tera a copia desse contador
			//que será o seu valor de nó referencia (ou seja, nós referencias de 1 a 15)
			no_ref = contador_no_ref+1;

			pid_fork = fork();

			//depois do fork o pai (no caso escalonador)
			//preenche a tabela de processos com os pids dos processos criados
			//suas referencias, pids e marca-os como livre
			if (pid_fork > 0)
			{
				tab_proc[contador_no_ref].pid = pid_fork;
				tab_proc[contador_no_ref].no_ref = contador_no_ref+1;
				tab_proc[contador_no_ref].livre = 1;

				contador_no_ref++;
			}
		}
		else
		{
			//se for o filho ele sai do for e nao faz fork
			break;
		}
		
	}


	//se NAO for o escalonador ele entra na funcao
	//que o faz ficar em busy wait esperando a ordem de execucao
	if (getpid() != pid_principal)
	{
		pid = getpid();
		executa_programa();
	}
}


/*
* Funcao que executa o programa recebido pelo escalonador
*/
void trata_mensagem(char *caminho_prog, char *programa)
{
	pid_t pid_exec;
	int status;

	//o processo cria um fork
	//e guarda o pid do filho
	pid_exec = fork();


//o filho faz um execl e executa o programa que recebeu
	if (pid_exec == 0)
	{
		//filho
      	execl(caminho_prog, programa, NULL);
		//execl("./teste.out", "teste", NULL);
      	perror("execl() failure!\n\n");

		exit(1);	
	}
	else
	{
		//o pai espera o filho terminar a execucao
		wait(&status);
	}
}


/*
* Funcao que coloca o processo em busy wait 
* esperando a ordem para executar algum programa
*/
void executa_programa(void)
{
	mensagem msg;

	//loop que o processo fica esperando chegar mensagem com
	//nome do programa para executar
	while(1)
	{
		//processo recebe mensagem com o nome do programa para executar
		if (msgrcv(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
			//se o tipo da mensagem for de execucao ele entra no if
			if(msg.operacao == TYPE_EXEC)
			{
				//ele muda o pid da mensagem recebida para o pid do processo que recebeu
				//e muda o no referencia para a referencia do no que recebeu a mensagem
			    msg.pid = pid;
			    msg.no_source = no_ref;

			    //entao envia a mensagem para ser 'roteada'
			    //ou seja, a mensagem vai ser passada para os próximos nós
		    	roteamento_msg(msg);


		    	//depois de passar a mensagem para os nós que precisa passar
		    	//o processo vai executar o programa
		    	//aqui ele pega o tempo de inicio e final da execucao para saber quanto tempo levou
		    	tempos[0] = time(NULL);
		    	trata_mensagem(msg.mtext, msg.prog);
		    	tempos[1] = time(NULL);


		    	//depois de executar ele envia os tempos de execucao
		    	//para o escolonador
				fim_programa(tempos[0], tempos[1]);
			}
		}
	}
}

/*
* Funcao que finaliza a tarefa de executar um programa
* ela envia uma mensagem com os tempos de inicio e fim
* que o processo levou para executar
*/
void fim_programa(unsigned long time_ini, unsigned long time_end)
{
	mensagem msg;
	int contador_roteamento = 0;


	//cria mensagem para enviar os tempos de execucao
	//com o mtype = a referencia do proprio no que está gerando a mensagem
	msg.mtype  = no_ref;
	msg.pid = pid;
	msg.no_source = no_ref;
	msg.no_dest = TYPE_ESC;
	msg.livre = true;
	msg.operacao = TYPE_FIN;
	msg.time_ini = time_ini;
	msg.time_end = time_end;
	(void) strcpy(msg.mtext,"no terminou exec") ;
	
	//envia mensagem
	if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}

	

	//loop que o processo fica para receber e repassar a mensagem final
	//com os tempos de execucao de cada processo
	while(1)
	{
		//recebe a mensagem que for para o processo
		//ele fica esperando uma mensagem com o mtype = seu no de referencia
		if (msgrcv(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
			//quando a mensagem chega e é do tipo fin entra no if
			if (msg.operacao == TYPE_FIN)
			{
				//usa a funcao de 'roteamento' para repassar as mensagens dos outros nos
		    	roteamento_msg(msg);

		    	contador_roteamento++;

		    	//se for uma folha da arvore ele sai do loop porque nenhum outro nó manda
		    	//mensagem por ele
		    	if (is_folha())
		    	{
		    		break;
		    	}
		    	else if (((no_ref == 2) ||  (no_ref == 9)) && (contador_roteamento == 7))
		    	{
		    		//se for o no 2 ou 9 ele repassa 7 mensagens e sai do loop
		    		break;
		    	}
		    	else if (((no_ref == 3) ||  (no_ref == 6) || (no_ref == 13) || (no_ref == 10)) && (contador_roteamento == 3))
				{
					//se for no 3, 6, 13 ou 10 eles repassam 3 mensagens e saem do loop
					break;
				}
		    	else if ((no_ref == 1) && (contador_roteamento == 15))
				{
					//se for o no 1 ele precisa repassar os 15 pacotes e sai do loop
					break;
				}
			}
		}
	}

	//printf("[FAT] ROTEEI TODAS AS MENSAGENS");
	fflush(stdout);
}


/*
* Funcao que repassa as mensagens para os nos
* de acordo com a topologia
*/
void roteamento_msg(mensagem msg)
{
	int type_all = 0; 
	int type_escalonador = 0;


	//se a mensagem for de destino = todos os processos, ele seta a flag 
	if(msg.no_dest == TYPE_ALL) type_all = 1;
	//se a mensagem do destino = escalonador, ele seta outra flag
	if(msg.no_dest == TYPE_ESC) type_escalonador = 1;


	// printf("[%d]PARA TODOS = %d -> %d\n",msg.operacao, no_ref, type_all);
	

	//switch para repassar as mensagems entre os nos
	switch(msg.mtype)
	{
		//se for 4 
		case TYPE_NO_4:
		case TYPE_NO_5:
			if (type_escalonador)
			{
				// enviar para no 3
				// printf("ROTEAR PARA NO 3 -> %d\n", no_ref);
				msg.mtype  = TYPE_NO_3;
				// msg.no_dest  = TYPE_NO_3;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_7:
		case TYPE_NO_8:
			if (type_escalonador)
			{
				//enviar para no 6
				// printf("ROTEAR PARA NO 6 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_6;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_14:
		case TYPE_NO_15:
			if (type_escalonador)
			{
				//enviar para no 13
				// printf("ROTEAR PARA NO 13 -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_NO_13;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_12:
		case TYPE_NO_11:
			if (type_escalonador)
			{
				//enviar para no 10
				// printf("ROTEAR PARA NO 10 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_10;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_3:
		case TYPE_NO_6:
			if (type_all)
			{
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				//enviar para no 2
				// printf("ROTEAR PARA NO 2 -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_NO_2;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_13:
		case TYPE_NO_10:
			if (type_all)
			{
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if (type_escalonador)
			{
				//enviar para no 1
				// printf("ROTEAR PARA NO 9 -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_NO_9;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_9:
		case TYPE_NO_2:
			if (type_all)
			{
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				
				msg.mtype += 3;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				//enviar para no 0
				// printf("ROTEAR PARA NO 1 -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_NO_1;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		default:
			if (type_all)
			{
				// printf("ROTEAR PARA NO 2 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+1;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				// printf("ROTEAR PARA NO 9 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+8;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				//printf("ROTEAR PARA ESC -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_ESC;
				if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
					//perror("Erro no envio da mensagem") ;
				}
			}
			break;
	}
}


//verificando se o processo é folha
//se for retorna 1 e entao ele nao precisa passar para baixo a mensagem
//e nem roteia para ninguem
int is_folha(void)
{
	for (int i = 0; i < 8; ++i)
	{
		if((no_ref == 4) || (no_ref == 5) || (no_ref == 7) || (no_ref == 8) || (no_ref == 14) || (no_ref == 15) ||
			(no_ref == 12) || (no_ref == 11))
			return 1;
	}

	return 0;
}



