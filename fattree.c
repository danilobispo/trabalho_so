#include <stdio.h> 
#include <stdlib.h>

#include "fattree.h"



void cria_fattree()
{
	int cont_no = 0; 
  	

  	pid_principal = getpid();
  	printf("PAI GER %d\n", pid_principal);


   	/*criando os processos*/
	for (cont_no = 0; cont_no < N_FORKS; cont_no++)
	{
		fork();
	}


	if (getpid() != pid_principal)
	{
		registra_processo();

		guarda_ref();

		//TODO:no 1 pega todos os PIDS e guarda em um vetor

		// printf("NO %d | %d\n", pid, no_ref);

		signal(SIGUSR1, executa_programa);
		signal(SIGUSR2, fim_programa);

		while(1);
	}
}


void guarda_ref(void)
{
	mensagem msg;

	while(msgrcv(msgid, &msg, TAM_TOTAL_MSG, getpid(), IPC_NOWAIT) < 0)
	{}

	no_ref = msg.no_dest;
}

void registra_processo(void)
{
	mensagem msg;

	pid = getpid();

    /*
	* enviado para o escalonador dados do processo
	* notificando que já foi criado e está livre
	*/
	msg.mtype  = TYPE_ESC; //coloca type_no_0 quando quer enviar para o escalonador
	msg.pid = pid;
	msg.no_source = pid; /*TYPE_NO_eu*/
	msg.no_dest = TYPE_ESC;
	msg.livre = true;
	(void) strcpy(msg.mtext,"no criado") ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}
}

//somente no 0 vai usar
void acorda_filhos(void)
{
	for (int i = 1; i < N_NOS; ++i)
	{
		kill(tab_proc[i].pid, SIGUSR1);
	}
}



void executa_programa(int signum)
{
	mensagem msg;

	if(no_ref == TYPE_1) acorda_filhos();

	while(1)
	{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
			// printf("NO %d\n", no_ref);
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
		    msg.pid = pid;
		    msg.no_source = no_ref;

	    	roteamento_msg(msg);

			printf("TRATA MENSAGEM = %d\n", getpid());

			// wait();

			kill(pid, SIGUSR2);

			break;

		    /* 
		    * se a mensagem não for para mim repassa
		    * se for, trata
		    */
		}

		// contador ++;
		// break;
	}
}


void fim_programa(int signum)
{
	mensagem msg;
	int contador_roteamento = 0;

	msg.mtype  = TYPE_1; //coloca type_no_0 quando quer enviar para o escalonador
	msg.pid = pid;
	msg.no_source = pid; /*TYPE_NO_eu*/
	msg.no_dest = TYPE_ESC;
	msg.livre = true;
	(void) strcpy(msg.mtext,"no terminou exec") ;

	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}

	while(1)
	{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
			// printf("NO %d\n", no_ref);
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
	    	roteamento_msg(msg);

	    	if ((contador_roteamento == 2) || is_folha())
	    	{
	    		break;
	    	}

	    	contador_roteamento++;
		}
	}
}


void roteamento_msg(mensagem msg)
{
	int type_all = 0; 
	if(msg.no_dest == TYPE_ALL) type_all = 1;
	
	switch(msg.no_source)
	{
		case TYPE_NO_4:
		case TYPE_NO_5:
			if (!type_all)
			{
				// enviar para no 3
				printf("ROTEAR PARA NO 3 -> %d\n", no_ref);
				msg.mtype  = TYPE_NO_3;
				msg.no_dest  = TYPE_NO_3;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_7:
		case TYPE_NO_8:
			if (!type_all)
			{
				//enviar para no 6
				printf("ROTEAR PARA NO 6 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_6;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_14:
		case TYPE_NO_15:
			if (!type_all)
			{
				//enviar para no 13
				printf("ROTEAR PARA NO 13 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_13;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_12:
		case TYPE_NO_11:
			if (!type_all)
			{
				//enviar para no 10
				printf("ROTEAR PARA NO 10 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_10;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_3:
		case TYPE_NO_6:

			if (type_all)
			{
				printf("ROTEAR PARA NO 4|7 -> %d\n", no_ref);
				msg.mtype += 1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				printf("ROTEAR PARA NO 5|8 -> %d\n", no_ref);
				msg.mtype += 2;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else{
				//enviar para no 2
				printf("ROTEAR PARA NO 2 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_2;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_13:
		case TYPE_NO_10:

			if (type_all)
			{
				printf("ROTEAR PARA NO 14|11 -> %d\n", no_ref);
				msg.mtype += 1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				printf("ROTEAR PARA NO 15|12 -> %d\n", no_ref);
				msg.mtype += 2;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else{
				//enviar para no 1
				printf("ROTEAR PARA NO 9 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_9;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		case TYPE_NO_9:
		case TYPE_NO_2:

			if (type_all)
			{
				printf("ROTEAR PARA NO 3|10 -> %d\n", no_ref);
				msg.mtype += 1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				printf("ROTEAR PARA NO 6|13 -> %d\n", no_ref);
				msg.mtype += 4;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else{
				//enviar para no 0
				printf("ROTEAR PARA NO 0 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		default:
			//enviar escalonador
			if (type_all)
			{
				printf("ROTEAR PARA NO 2 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				printf("ROTEAR PARA NO 9 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+8;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else
			{
				printf("ROTEAR PARA ESC -> %d\n", no_ref);
				msg.mtype = TYPE_ESC;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}

			break;
	}
}

int is_folha(void)
{
	for (int i = 0; i < 8; ++i)
	{
		if(vetor_folhas[i] == no_ref)
			return 1
	}

	return 0;
}



