#include "escalonador.h"


int main(int argc, char const *argv[])
{
	msg_postergado msg;

	// Pegando a topologia e verificando formato
	if ((argc > 2) || (argc < 2)){
		printf("\nFORMATO INVÁLIDO 1.\n");
		exit(1);
	}
	else
	{
		if (!strcmp(argv[1],"F") || !strcmp(argv[1],"T") || !strcmp(argv[1],"H"))
		{
			topologia = (char)*argv[1];
		}
		else{
			printf("\nFORMATO INVÁLIDO.\n");
			exit(1);
		}
		
	}
	//---------------------------
	
	
	//------------INICIO ESCALONADOR--------

	inicia_tab_jobs();

	cria_fila_mensagem_postergado();

	msg.mtype  = TYPE_ESC_JOBS;
	msg.pid = getpid();
	msg.job = cont_job;
	msg.tempo_delay = 0;
	(void) strcpy(msg.nome_programa,"mensagem contador job");
	msgsnd(msgid_postergado, &msg, sizeof(msg), 0);
	//----------------------------------

	

	//inicializa a tologia depois da escolha
	inicializa_topologia_escolhida();

	printf("CHEGUEI AQUI 65565");
	fflush(stdout);

	

	//fica em loop esperando mensagem do executa postergado
	//e manda executar o programa na hora
	recebi_msg_postergado();

	

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
    key_fila_msg = KEY_ESC_POSTERGADO;

	if (( msgid_postergado = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
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

	printf("CHEGUEI AQUI 77777");
	fflush(stdout);

	while(1){


		espera_execucao_job();


		if (msgrcv(msgid_postergado, &msg, TAM_TOTAL_MSG, TYPE_ESC_EXEC, IPC_NOWAIT) < 0) {
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
			if(msgsnd(msgid_postergado, &msg, sizeof(msg), 0)<0){
				perror("[escalonador]Erro de msgget") ;
			}

		}
	}
	printf ("chegou aqui");
}


//funcao para ordenar a execucao de um programa
void espera_execucao_job (void){
	time_t temp_agora = time(NULL);
	int status;


	//caso seja a hora certa manda executar o programa
	if (temp_agora == tab_job[menor_job].tempo_futuro){

		printf("Tempo agora = %ld | Tempo futuro = %ld", temp_agora, tab_job[menor_job].tempo_futuro);
		fflush(stdout);

		tab_job[menor_job].executado = 1;
		menor_delay = 10000;

		if(topologia == 'F')
		{

			//organiza as infos da fila de acordo com cada topologia
			//se precisar cria, ou salva o id da fila
			infos_fila_msg();

			//envia mensagem para a fila para executar o programa
			aciona_execucao_prog(tab_job[menor_job].nome_programa, "a", N_NOS_FATTREE);
		}
		else if (topologia == 'H')
		{
			//setando todos os processos como livres
			//pois a execucao ainda nao comecou
			for (int i = 0; i < 16; ++i)
			{
				tab_proc[i].livre = 1;
			}

			//o escalonador se foerkeia para chamar a topologia
			int pid_hyper = fork();

			//se for o escalonador ele manda executar o programa
			if (pid_hyper != 0)
			{
				//organiza as infos da fila de acordo com cada topologia
				//se precisar cria, ou salva o id da fila
				infos_fila_msg();

				//envia mensagem para a fila para executar o programa
				aciona_execucao_prog(tab_job[menor_job].nome_programa, "a", N_NOS_CUBO_TORUS);

				wait(&status);
			}
			else
			{
				execl("./hipercubo", "a", NULL);
			}
		}
		else
		{
			//envia mensagem para a fila para executar o programa
			//aciona_execucao_prog(tab_job[menor_job].nome_programa, "a", N_NOS_FATTREE);
		}
	}
}


void inicializa_topologia_escolhida(void)
{
	if(topologia == 'H')
	{
		printf("Topologia Hyper cubo");

		start_hyper_cubo();
	}
	else if (topologia == 'T')
	{
		printf("Topologia Torus");

		start_torus();
	}
	else if (topologia == 'F')
	{
		printf("Topologia Fat tree");

		start_fat_tree();
	}
	
}


void start_hyper_cubo(void)
{
	//cria a topologia com a chamada de codigo da helena

	printf("Nao tem esse passo");


}

void start_torus(void)
{
	//cria a topologia com a chamada de codigo do danilo
}

void start_fat_tree(void)
{
	//cria a topologia com a chamada de codigo da lais
}


/*
* Essa funcao envia a mensagem com o nome do programa a ser executado
* e marca os processos como ocupados
*/
void ordem_executa_programa(char *caminho_prog, char *programa, int n_nos)
{
	mensagem msg;

    /*
	* enviado para o escalonador dados do processo
	* notificando que já foi criado e está livre
	*/
	msg.mtype  = TYPE_NO_1;
	msg.pid = pid_principal;
	msg.no_source = TYPE_ESC;
	msg.no_dest = TYPE_ALL;
	msg.livre = false;
	msg.operacao = TYPE_EXEC;
	(void) strcpy(msg.mtext,caminho_prog) ;
	(void) strcpy(msg.prog,programa) ;

	if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
	}


	for (int i = 0; i < n_nos; ++i)
	{
		tab_proc[i].livre = 0;
	}
}

//organiza as infos das filas de acordo com cada topologia
void infos_fila_msg(void)
{
	if(topologia == 'H')
	{
		while(1){
			/*  recuperacao do id da fila de mensagens da topologia hypercubo      */
			if ((msgid_fila_topologia = msgget(100012313,0)) == -1) {
				// perror("Erro na criacao da fila do hyper cubo") ;
			}
			else
			{
				break;
			}
		}
		
	}
	else if (topologia == 'T')
	{

	}
	else if (topologia == 'F')
	{

	}
}

int is_todos_livres(int n_nos)
{
	for (int i = 0; i < n_nos; ++i)
	{
		printf("tab[%d].livre = %d",i, tab_proc[i].livre);
		if (tab_proc[i].livre == 0)
		{
			return 0;
		}
	}

	return 1;
}


void aciona_execucao_prog(char *caminho_prog, char *programa, int n_nos)
{
	printf("VOU EXECUTAR | nos = %d\n", n_nos);
	fflush(stdout);

	if (is_todos_livres(n_nos))
	{
		printf("[ESC]EXECUTANDO...\n");
		fflush(stdout);
		ordem_executa_programa(caminho_prog, programa, n_nos);
		
		printf("[ESC]ESPERANDO RESULTADO...");
		fflush(stdout);
		espera_resultado_execucao(n_nos);
	}
	else
	{
		printf("Não posso executar. Nem todos os processos estão livres!\n");
		fflush(stdout);
	}
}

/*
* Essa funcao recebe a resposta dos processos
* e guarda as informações na tabela de processos
*/
void espera_resultado_execucao(int n_nos)
{
	int contador_no_ref = 0;
	int index;
	mensagem msg;

	while(contador_no_ref < n_nos)
	{
		if (msgrcv(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, TYPE_ESC, IPC_NOWAIT) < 0) {
		//    perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{
			// if (msg.operacao == TYPE_FIN)
			// {
				index = search_proc(msg.pid, 2);

				if (index != -1)
				{
					printf("[ESCALONADOR]PROCESSO %d finalizou | %lu -> %lu\n", tab_proc[index].no_ref, msg.time_ini, msg.time_end);
					marca_gerente_livre(msg.pid, msg.time_ini, msg.time_end, n_nos);

					contador_no_ref++;
				}
			// }
		}
	}
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

void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end, int n_nos)
{
	for (int i = 0; i < n_nos; ++i)
	{
		if (tab_proc[i].pid == ref)
		{
			tab_proc[i].livre = 1;
			tab_proc[i].time_ini = time_ini;
			tab_proc[i].time_end = time_end;
		}
	}
}