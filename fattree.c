#include <stdio.h> 
#include <stdlib.h>

#include "fattree.h"



void cria_fattree()
{
	int cont_no = 0; 
  	

  	pid_principal = getpid();


   	/*criando os processos*/
	for (cont_no = 0; cont_no < N_FORKS; cont_no++)
	{
		fork();
	}


	if (getpid() != pid_principal)
	{
		//---------------------------------------------
		/*
		* boloc de codigo que cria tudo que é necessario
		* para a topologia funcionar
		*/
		registra_processo();

		guarda_ref();
		
		comunica_no_raiz();


		executa_programa();
		//---------------------------------------------
	}
}



void comunica_no_raiz(void)
{
	mensagem msg, msg_rcv;
	int contador_nos = 0;

	// if (no_ref != TYPE_NO_1)
	// {
	    /*
		* enviado para o escalonador dados do processo
		* notificando que já foi criado e está livre
		*/
		msg.mtype  = TYPE_NO_1;
		msg.pid = pid;
		msg.no_source = no_ref;
		msg.no_dest = TYPE_NO_1;
		msg.livre = true;
		msg.operacao = 0;
		(void) strcpy(msg.mtext,"comunica no 1") ;


		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
		   perror("[CRIANDO]Erro no envio da mensagem");
		}
	// }
	if (no_ref == TYPE_NO_1)
	{
		while(contador_nos < N_NOS)
		{
			if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_NO_1, IPC_NOWAIT) < 0)
			{
				// perror("[TRANSMITE]Erro na recepcao da mensagem");
			}
			else
			{
				if (msg.operacao == 0)
				{
					nos_vetor[contador_nos].no_ref = msg.no_source;
					nos_vetor[contador_nos].pid = msg.pid;
					// printf("[PROC 1]VETOR %d | %d\n", nos_vetor[contador_nos].no_ref, nos_vetor[contador_nos].pid);
					contador_nos++;
				}
			}
		}

		//verificando se tudo o setup terminou
		msg.mtype  = TYPE_ESC;
		msg.pid = pid;
		msg.no_source = no_ref;
		msg.no_dest = TYPE_ESC;
		msg.livre = true;
		msg.operacao = 0;
		(void) strcpy(msg.mtext,"topologia feita!") ;
		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
		   perror("[CRIANDO]Erro no envio da mensagem");
		}
		//--------------------------------------
	}
}


void guarda_ref(void)
{
	mensagem msg;

	while(msgrcv(msgid, &msg, TAM_TOTAL_MSG, getpid(), IPC_NOWAIT) < 0)
	{}

	if (msg.operacao == TYPE_INI)
	{
		no_ref = msg.no_dest;
	}
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
	msg.operacao = TYPE_INI;
	msg.livre = true;
	(void) strcpy(msg.mtext,"no criado") ;


	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}
}


void trata_mensagem(char *caminho_prog, char *programa)
{
	pid_t pid_exec;
	int status;

	pid_exec = fork();

	if (pid_exec == 0)
	{
		//filho
      	execl(caminho_prog, programa, 0, 0);
      	perror("execl() failure!\n\n");

		exit(1);	
	}
	else
	{
		//pai
		wait(&status);
	}
}

void executa_programa(void)
{
	mensagem msg;

	while(1)
	{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
			if(msg.operacao == TYPE_EXEC)
			{
			    msg.pid = pid;
			    msg.no_source = no_ref;

		    	roteamento_msg(msg);

		    	tempos[0] = clock();
		    	trata_mensagem(msg.mtext, msg.prog);
		    	tempos[1] = clock();

				fim_programa(tempos[0], tempos[1]);
			}
		}
	}
}


void fim_programa(unsigned long time_ini, unsigned long time_end)
{
	mensagem msg;
	int contador_roteamento = 0;

	msg.mtype  = no_ref;
	msg.pid = pid;
	msg.no_source = no_ref;
	msg.no_dest = TYPE_ESC;
	msg.livre = true;
	msg.operacao = TYPE_FIN;
	msg.time_ini = time_ini;
	msg.time_end = time_end;
	(void) strcpy(msg.mtext,"no terminou exec") ;
	
	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}


	while(1)
	{
		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem") ;
		}
		else
		{
			if (msg.operacao == TYPE_FIN)
			{
		    	roteamento_msg(msg);

		    	contador_roteamento++;

		    	if (is_folha())
		    	{
		    		break;
		    	}
		    	else if (((no_ref == 2) ||  (no_ref == 9)) && (contador_roteamento == 7))
		    	{
		    		break;
		    	}
		    	else if (((no_ref == 3) ||  (no_ref == 6) || (no_ref == 13) || (no_ref == 10)) && (contador_roteamento == 3))
				{
					break;
				}
		    	else if ((no_ref == 1) && (contador_roteamento == 15))
				{
					break;
				}
			}
		}
	}
}


void roteamento_msg(mensagem msg)
{
	int type_all = 0; 
	int type_escalonador = 0;
	if(msg.no_dest == TYPE_ALL) type_all = 1;
	if(msg.no_dest == TYPE_ESC) type_escalonador = 1;
	// printf("[%d]PARA TODOS = %d -> %d\n",msg.operacao, no_ref, type_all);
	
	switch(msg.mtype)
	{
		case TYPE_NO_4:
		case TYPE_NO_5:
			if (type_escalonador)
			{
				// enviar para no 3
				// printf("ROTEAR PARA NO 3 -> %d\n", no_ref);
				msg.mtype  = TYPE_NO_3;
				// msg.no_dest  = TYPE_NO_3;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
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
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
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
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
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
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
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
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				//enviar para no 2
				// printf("ROTEAR PARA NO 2 -> %d | %d\n", no_ref, msg.pid);
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
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if (type_escalonador)
			{
				//enviar para no 1
				// printf("ROTEAR PARA NO 9 -> %d | %d\n", no_ref, msg.pid);
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
				msg.mtype += 1;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				
				msg.mtype += 3;
				// printf("ROTEAR PARA NO %ld -> %d\n",msg.mtype, no_ref);
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				//enviar para no 0
				// printf("ROTEAR PARA NO 1 -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_NO_1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			break;
		default:
			if (type_all)
			{
				// printf("ROTEAR PARA NO 2 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+1;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
				// printf("ROTEAR PARA NO 9 -> %d\n", no_ref);
				msg.mtype = TYPE_NO_1+8;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					// perror("Erro no envio da mensagem") ;
				}
			}
			else if(type_escalonador)
			{
				// printf("ROTEAR PARA ESC -> %d | %d\n", no_ref, msg.pid);
				msg.mtype = TYPE_ESC;
				if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
					perror("Erro no envio da mensagem") ;
				}
			}
			break;
	}
}

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



