#include <stdio.h> 
#include <stdlib.h>

#include "fattree.h"



void cria_fattree()
{
	int cont_no = 0;
  	
  	char *char_pid;
  	int msgid;
  	mensagem msg;


    /*
     * criacao de uma fila de mensagens para leitura se
     * ela ainda nao existe
     */
	if ((msgid = msgget(key_fila_msg, 0)) == -1) {
		perror("Erro na criacao da fila do servidor") ;
	    // exit(1) ;
	}
   	printf("identificador da fila: %d\n",msgid) ;
   	// printf("esta fila esta associada a chave unica : %#x\n",ftok(path,(key_t)KEY)) ;


   	//criando os processos
	for (cont_no = 0; cont_no < N_NOS; cont_no++)
	{

		//se for pai apenas sai do for
    	if ((pid = fork()) == 0)
		{
			//printf("Sou pai pid=%d\n", getpid());
			break;
		}
		//se for filho adiciona o id na lista de mapeamento da arvore
		else
		{
			printf("%d\n", cont_no);
			printf("Sou filho pid=%d\n", pid);

			if (cont_no == 0)
			// {
			// 	close(tubo_esc_no[0]);
			// 	close(tubo_esc_no[1]);

			// 	close(tubo_no_esc[0]);
			// 	close(tubo_no_esc[1]);
			// }
			// else
			{
				// sprintf(char_pid,"%d",pid);

		        /*        envio de mensagem         */
		        msg.mtype  = 0 ;
		        msg.pid = pid;
		        msg.livre = true;
		        strcpy(msg.mtext[],"teste") ;
		        // msg.mtext = "teste";
		        // printf("mandei mensagem\n");
		        if (msgsnd(msgid, &msg, TAM_TOTAL_MSG , 0) == -1) {
		               perror("Erro no envio da mensagem") ;
		               // exit(1) ;
		        }



				// write(tubo_no_esc[1], char_pid, sizeof(char_pid) + 1);
				// signal(SIGUSR1, mensagem_no_raiz);
				// printf("enviou sinal\n");
			}

			arvore[cont_no].id_pid = pid;
			arvore[cont_no].id_no = cont_no;
		}
	}




	//print_arvore(arvore);

	/* code */
	// return 0;
}


void mensagem_no_raiz(int signum)
{
	if (getpid() == arvore[0].id_pid)
	{
		read(tubo_esc_no[0], msg_recebida, sizeof(msg_recebida));
		printf("[%d]RECEBI MSG %s\n", getpid(), msg_recebida);
	}
}


void print_arvore(arvore_processos arvore[])
{
	for (int i = 0; i < 15; ++i)
	{
		printf("arvore[%d] -> %d / %d \n", i, arvore[i].id_pid, arvore[i].id_no);
	}
}

