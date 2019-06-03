#include "escalonador.h"
#include "fattree.h"


int main(int argc, char const *argv[])
{
	msg_postergado msg;
	const char delimitador_barra[2] = "/";
   	char *token;

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
	
	//------------SHUTDOWN----------------
		
		//SINAL NECESSARIO PARA O SHUTDOWN
		signal(SIGUSR1, the_end);

		cria_fila_shutdown();
	
	//------------------------------------


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




	//fica em loop esperando mensagem do executa postergado
	//e manda executar o programa na hora
	recebi_msg_postergado();

	

	while(1);
	
	return 0;
}


/*
* criacao de uma fila para o shutdown
*/
void cria_fila_shutdown(void)
{
	mensagem msg;
    key_fila_msg = KEY_ESC_SHUT;

	if (( msgid_shutdown = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
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

		if (msgsnd(msgid_shutdown, &msg, TAM_TOTAL_MSG, 0) < 0) {
			perror("[ESCALONADOR]Erro no envio da mensagem") ;
		}
		////////
	}


	printf("[ESCALONADOR] ID SHUTDOWN = %d\n", msgid_shutdown);
	fflush(stdout);


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

	printf("[ESCALONADOR] ID POSTERGADO = %d\n", msgid_postergado);
	fflush(stdout);
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


		if (msgrcv(msgid_postergado, &msg, TAM_TOTAL_MSG, TYPE_ESC_EXEC, IPC_NOWAIT) < 0) {
		    //perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
		}
		else
		{	programa_executado = 0;


			printf("\nJob: %d\n", msg.job);
			printf("Delay: %d\n", msg.tempo_delay);
			printf("Nome: %s\n", msg.nome_programa);
			printf("Type %ld\n", msg.mtype);

			strcpy(tab_job[cont_job].nome_programa,msg.nome_programa);

			//quebrando string
			const char delimitador_barra[2] = "/";
			const char delimitador_ponto[2] = ".";
  			char *token;
			char nome_programa_aux[100];
			token = strtok(msg.nome_programa, delimitador_barra);
			token = strtok(NULL, delimitador_barra);
			strcpy (nome_programa_aux,token);
			token = strtok(nome_programa_aux, delimitador_ponto);

			strcpy(tab_job[cont_job].nome_programa_sem_path,token);
			
			tab_job[cont_job].tempo_delay = msg.tempo_delay;
			tab_job[cont_job].inicio = time(NULL);
			tab_job[cont_job].tempo_futuro = tab_job[cont_job].tempo_delay + tab_job[cont_job].inicio;
			tab_job[cont_job].jobs = msg.job;
			tab_job[cont_job].executado = 0;

			busc_prox_prog();

			cont_job++;
			msg.job = cont_job;
			msg.mtype = TYPE_ESC_JOBS;
			if(msgsnd(msgid_postergado, &msg, sizeof(msg), 0)<0){
				perror("[escalonador]Erro de msgget") ;
			}

		}
	}
}


//funcao para ordenar a execucao de um programa
void espera_execucao_job (void){
	time_t temp_agora = time(NULL);
	int status;


	//caso seja a hora certa manda executar o programa
	if (temp_agora == tab_job[menor_job].tempo_futuro){

		if(!programa_executado)
		{
			tab_job[menor_job].executado = 1;
			menor_delay = 10000;
			programa_executado = 1;

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

					printf("[ESCALONADOR]EXECUTA AQUI");

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
}


void inicializa_topologia_escolhida(void)
{
	if(topologia == 'H')
	{
		//printf("Topologia Hyper cubo");

		start_hyper_cubo();
	}
	else if (topologia == 'T')
	{
		printf("Topologia Torus");

		start_torus();
	}
	else if (topologia == 'F')
	{
		//printf("Topologia Fat tree");

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
	
	/*
	* ORDEM DAS FUNCOES PARA A TOPOLOGIA FATTREE 
	*/
	//funcao inicia a topologia e deixa-a pronta pra uso
	inicializa_fattree();
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

/*
* criacao de uma fila de mensagens para leitura se
* ela ainda nao existe
*/
void cria_fila_mensagem(void)
{
	mensagem msg;
    key_fila_msg = KEY_FAT_TREE;

	if (( msgid_fila_topologia = msgget(key_fila_msg, IPC_CREAT|0666)) == -1) {
	     perror("Erro de msgget") ;
	}
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
	if(topologia == 'H')
	{
		msg.mtype  = TYPE_NO_1;
		msg.pid = cont_job-1;
		msg.no_source = TYPE_ESC;
		msg.no_dest = TYPE_ALL;
		msg.livre = false;
		msg.operacao = TYPE_EXEC;
		(void) strcpy(msg.mtext,caminho_prog) ;
		(void) strcpy(msg.prog,programa) ;
	}
	else
	{
		msg.mtype  = TYPE_NO_1;
		msg.pid = pid_principal;
		msg.no_source = TYPE_ESC;
		msg.no_dest = TYPE_ALL;
		msg.livre = false;
		msg.operacao = TYPE_EXEC;
		(void) strcpy(msg.mtext,caminho_prog) ;
		(void) strcpy(msg.prog,programa) ;
	}
	

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

		printf("[ESCALO0NADOR] id fila PRIMEIRO = %d\n", msgid_fila_topologia);
		fflush(stdout);
		
	}
	else if (topologia == 'T')
	{

	}
	else if (topologia == 'F')
	{
		/*  recuperacao do id da fila de mensagens da topologia hypercubo      */
		if ((msgid_fila_topologia = msgget(KEY_FAT_TREE,0)) == -1) {
			// perror("Erro na criacao da fila do hyper cubo") ;
		}

	}
}

int is_todos_livres(int n_nos)
{
	for (int i = 0; i < n_nos; ++i)
	{
		if (tab_proc[i].livre == 0)
		{
			return 0;
		}
	}

	return 1;
}


void aciona_execucao_prog(char *caminho_prog, char *programa, int n_nos)
{
	if (is_todos_livres(n_nos))
	{
		ordem_executa_programa(caminho_prog, programa, n_nos);
		
		printf("VOU EXPERAR MENSAGENS\n");
		fflush(stdout);

		if (topologia != 'H')
		{
			espera_resultado_execucao(n_nos);
		}
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
		    //perror("[ESCALONADOR]Erro na recepcao da mensagem") ;
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
	tab_job[cont_job-1].makespan = maior_tempo_fim - menor_tempo_inicio;
	printf("[ESCALONADOR]JOB %d | MAKESPAN %lu\n", cont_job-1, tab_job[cont_job-1].makespan);
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
	if(menor_tempo_inicio > time_ini)
	{
		menor_tempo_inicio = time_ini;
	}

	if (maior_tempo_fim < time_end)
	{
		maior_tempo_fim = time_end;
	}


	if (topologia != 'H')
	{
		for (int i = 0; i < n_nos; ++i)
		{
			if (tab_proc[i].pid == ref)
			{
				tab_proc[i].livre = 1;
			}
		}
	}
}



void the_end(int sig)
{
	printf("MATA TODO MUNDO!!\n");

	if (topologia == 'F')
	{
		for (int i = 0; i < N_NOS_FATTREE; ++i)
		{
			kill(tab_proc[i].pid, SIGKILL);
		}

		exclui_fila_mensagem(msgid_fila_topologia);
	}
	else if	(topologia != 'H')
	{
		for (int i = 0; i < N_NOS_CUBO_TORUS; ++i)
		{
			kill(tab_proc[i].pid, SIGKILL);
		}

		exclui_fila_mensagem(msgid_fila_topologia);
	}
	

	//no final da execucao do escalonador
	//a fila de msnagem deve ser excluida
	//ATENCAO SOMENTE NO FINAL NAO ANTES
	//SE NAO A TOPOLOGIA NAO FUNCIONARA MAIS
	exclui_fila_mensagem(msgid_shutdown);

	exclui_fila_mensagem(msgid_postergado);

	exit(1);
}



/*
* exclui a fila de mensagem
*/
void exclui_fila_mensagem(int id_da_fila)
{
	if( msgctl(id_da_fila, IPC_RMID, 0) < 0)
	{
		perror("[ESCALONADOR]Erro na exclusao da fila");
	}
	else
	{
		printf("EXCLUI LISTA DE MENSAGEM\n");
	}
}