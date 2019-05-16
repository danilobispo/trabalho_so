#include <stdio.h> 
#include <stdlib.h>

#include "fattree.h"



void cria_fattree()
{
	int cont_no = 0; 
  	

  	pid_principal = getpid();
  	printf("PAI GER %d\n", pid_principal);


   	/*criando os processos*/
	for (cont_no = 0; cont_no < N_NOS; cont_no++)
	{
		fork();
	}


	if (getpid() != pid_principal)
	{
		registra_processo();

		guarda_ref();
	}
}


void guarda_ref(void)
{
	mensagem msg;

	while(msgrcv(msgid, &msg, TAM_TOTAL_MSG, getpid(), IPC_NOWAIT) < 0)
	{}

	no_ref = msg.no_source;
	printf("NO %d | REF %d | PID %d\n", getpid(), msg.no_source, msg.pid);
}

void registra_processo(void)
{
	mensagem msg;

	pid = getpid();

	printf("HW %d\n", pid);

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




// void transmite_msg(mensagem msg)
// {
	// int no_ref = busca_no_ref(pid);
	// no_ref++;
	

	// int contador =0;
	// while(contador < 15)
	// {
	// 	if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	// 		// printf("NO %d\n", no_ref);
	//    		perror("[TRANSMITE]Erro na recepcao da mensagem") ;
	// 	}
	// 	else
	// 	{
		    
	// 	    * se a mensagem não for para mim repassa
	// 	    * se for, trata
		    
	// 		if (no_ref == msg.no_dest)
	// 		{
	// 			/* code */
	// 			printf("TRATA MENSAGEM = %d\n", getpid());
	// 			break;
	// 		}
	// 		else
	// 		{
	// 			roteamento_msg(msg);
	// 		}

	// 	}

	// 	contador ++;
	// 	// break;
	// }
// }



// void roteamento_msg(mensagem msg)
// {
	// printf("NO SOURCE %d\n", msg.no_source);
	// switch(msg.no_source)
	// {
	// 	case TYPE_NO_14:
	// 	case TYPE_NO_15:
	// 		// enviar para no 6
	// 		printf("ROTEAR PARA NO 6 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype  = TYPE_NO_7;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_12:
	// 	case TYPE_NO_13:
	// 		//enviar para no 5
	// 		printf("ROTEAR PARA NO 5 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_6;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_10:
	// 	case TYPE_NO_11:
	// 		//enviar para no 4
	// 		printf("ROTEAR PARA NO 4 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_5;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_8:
	// 	case TYPE_NO_9:
	// 		//enviar para no 3
	// 		printf("ROTEAR PARA NO 3 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_4;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_6:
	// 	case TYPE_NO_7:
	// 		//enviar para no 2
	// 		printf("ROTEAR PARA NO 2 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_3;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_4:
	// 	case TYPE_NO_5:
	// 		//enviar para no 1
	// 		printf("ROTEAR PARA NO 1 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_2;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	case TYPE_NO_2:
	// 	case TYPE_NO_3:
	// 		//enviar para no 0
	// 		printf("ROTEAR PARA NO 0 -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_NO_1;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// 	default:
	// 		//enviar escalonador
	// 		printf("ROTEAR PARA ESC -> %d\n", busca_no_ref(pid)+1);
	// 		msg.mtype = TYPE_ESC;
	// 		if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
	// 			perror("Erro no envio da mensagem") ;
	// 		}
	// 		break;
	// }
// }





