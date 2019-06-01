#include "escalonador.h"


int main(int argc, char const *argv[])
{
	msg_postergado msg;

	//------------INICIO ESCALONADOR--------

	inicia_tab_jobs();

	cria_fila_mensagem_postergado();

	msg.mtype  = TYPE_ESC_JOBS;
	msg.pid = getpid();
	msg.job = cont_job;
	msg.tempo_delay = 0;
	(void) strcpy(msg.nome_programa,"mensagem contador job");
	msgsnd(msgid, &msg, sizeof(msg), 0);

	recebi_msg_postergado();

	//----------------------------------

	while(1);
	
	return 0;
}


/*
* criacao de uma fila de mensagens para leitura se
* ela ainda nao existe
*/

void cria_fila_mensagem_postergado(void)
{
	msg_postergado msg;
    key_fila_msg = 123;

	if (( msgid = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
	     perror("Erro de msgget") ;
	}
}

void inicia_tab_jobs(void){
	int i;
	for (i=0; i<100; i++){
		tab_job[i].jobs = -1;
		tab_job[i].executado = 0;
		tab_job[i].makespan = 0;
	}
}

void busc_prox_prog (void){
	int i=0;
	
	while(tab_job[i].jobs != -1){
		if(menor_delay > tab_job[i].tempo_delay && tab_job[i].executado == 0){
			menor_job = tab_job[i].jobs;
			menor_delay =  tab_job[i].tempo_delay;
		}
		i++;
	}
}

void recebi_msg_postergado (void){

	msg_postergado msg;
	time_t tempo_inicio;

	while(1){


		espera_execucao_job();


		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC_EXEC, IPC_NOWAIT) < 0) {
		    //perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{	
			strcpy(tab_job[cont_job].nome_programa,msg.nome_programa);
			tab_job[cont_job].tempo_delay = msg.tempo_delay;
			tab_job[cont_job].inicio = time(NULL);
			tab_job[cont_job].tempo_futuro = tab_job[cont_job].tempo_delay + tab_job[cont_job].inicio;
			tab_job[cont_job].jobs = msg.job;
			tab_job[cont_job].executado = 0;

			busc_prox_prog();

			printf("\nJob: %d\n", msg.job);
			printf("Delay: %d\n", msg.tempo_delay);
			printf("Nome: %s\n", msg.nome_programa);
			printf("Type %ld\n", msg.mtype);

			cont_job++;
			msg.job = cont_job;
			msg.mtype = TYPE_ESC_JOBS;
			if(msgsnd(msgid, &msg, sizeof(msg), 0)<0){
				perror("[escalonador]Erro de msgget") ;
			}

		}
	}
	printf ("chegou aqui");
}


void espera_execucao_job (void){
	time_t temp_agora = time(NULL);
	if (temp_agora == tab_job[menor_job].tempo_futuro){

		tab_job[menor_job].executado = 1;
		menor_delay = 10000;
		//printf("tempo agora: %ld",temp_agora/1000);
	}
}
