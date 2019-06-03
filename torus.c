//
// Created by oem on 24/05/19.
//

#include "torus.h"

// int main()
// {
//     printf("Topologia Torus selecionada\n");
//     inicializaTorus();

//     /*
//     * funcao que cria os processos
//     */
//     criaProcessosTorus();
//     aciona_execucao_prog("./a.out", "a");
//     while (1);
//     return 0; 
// }

/**
	Verifica se vizinho a ser adicionado ja nao existe no vetor de vizinhos daquele no
	**/
int isVizinhoDuplicado(int noAtual, int vizinho)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (noTorus[noAtual].vizinhos[i] == vizinho)
        {
            return 1; // Se for duplicado retorna true;
        }
    }
    return 0; // Else retorna false;
}

/**
Adiciona vizinho diretamente
**/
void adicionarVizinho(int noAtual, int vizinho)
{
    int j = 0;
    int adicionou = 0;
    while (!adicionou && j < 4)
    {
        if (noTorus[noAtual].vizinhos[j] == -1)
        {                                           // Se a posicao j estiver vazia...
            noTorus[noAtual].vizinhos[j] = vizinho; // Adiciona novo vizinho
            adicionou = 1;
        }
        j++;
    }
    if (adicionou == 0)
    {
        //        DEBUG
        //        printf("Erro, no ja possui o maximo de vizinhos! No: %d\n", noAtual);
        //        printf("Vizinho que nao foi adicionado: %d\n", vizinho);
    }
}

/**
Informa vizinho que no x eh vizinho, vizinho entao adiciona o dado no, caso nao seja duplicado
**/
void informarVizinho(int noAtual, int noIdVizinho)
{
    if (!isVizinhoDuplicado(noAtual, noIdVizinho))
    {
        adicionarVizinho(noAtual, noIdVizinho);
    }
}

/**
Imprime os nos do torus, so serve pra debug atualmente
**/
void imprimeNosTorus()
{
    int i = 0;
    int j = 0;
    char livre[4];

    for (i; i < 16; i++)
    {
        printf("No atual: %d\n", noTorus[i].noId);
        noTorus[i].livre ? stpcpy(livre, "Sim") : strcpy(livre, "Nao");
        printf("livre? %s\n", livre);
        for (j = 0; j < 4; j++)
        { //Imprime n?s vizinhos
            printf("Nos vizinhos: %d\n", noTorus[i].vizinhos[j]);
        }
    }
}

/**
	Inicializa os nos do torus
	**/
void criaNosTorus()
{
    int i = 0;
    int j = 0;
    int m = 0;
    int n = 4;
    int fork0, fork1, fork2, fork3;
    int forkcircularvertical;
    int forkcircularhorizontal;

    // Visão abstrata dos processos, nenhum fork foi realizado por ora
    for (i; i < n_nos_topologia; i++)
    {
        noTorus[i].noId = i;
        noTorus[i].livre = 1; // A principio, todos os nos estao livres
        for (j = 0; j < 4; j++)
        {
            noTorus[i].vizinhos[j] = -1; // Vizinho invalido, quando a insercao for realizada
            // esse valor sera necessario para checar se eu nao estou sobrescrevendo nada
        }
    }

    // Zerando as variáveis para um novo loop:
    i = 0;
    j = 0;

    // Aqui adicionamos os vizinhos a todos os nós
    for (i = 0; i < n; i++)
    {
        //        DEBUG
        //        printf("Fila %d\n", i);
        for (j = 0; j < n; j++)
        {
            fork0 = m - 1; // Vizinho da esquerda
            fork1 = m + 1; // Vizinho da direita
            fork2 = m + 4; // Vizinho abaixo
            fork3 = m - 4; // Vizinho do topo
                           //            DEBUG
                           //            printf("Nó atual: %d\t", m);
            if (j != n - 1)
            {
                //                printf("Vizinho direita: %d(%d+1)\n", fork1, m);
                //adicionarVizinho(m, fork1);
                informarVizinho(fork1, m);
            }
            if (i != n - 1)
            {
                //                DEBUG
                //                printf("Vizinho abaixo: %d(%d+4)\n", fork2, m);
                adicionarVizinho(m, fork2);
                informarVizinho(fork2, m);
            }

            if (i == 0)
            { // Primeira fila
                forkcircularvertical = m + 12;
                //                DEBUG
                //                printf("Vizinho Circular Vertical: %d(%d+12)\n", forkcircularvertical, m);
                adicionarVizinho(m, forkcircularvertical);
                informarVizinho(forkcircularvertical, m);
            }
            if (j == 0)
            {
                forkcircularhorizontal = m + 3;
                //                DEBUG
                //                printf("Vizinho Circular Horizontal: %d(%d+3)\n", forkcircularhorizontal, m);
                adicionarVizinho(m, forkcircularhorizontal);
                informarVizinho(forkcircularhorizontal, m);
            }

            if ((fork0) >= 0)
            {
                //                DEBUG
                //                printf("Vizinho esquerda: %d(%d-1)\n", fork0, m);
                informarVizinho(fork0, m);
            }

            if ((fork3) >= 0)
            {
                //                DEBUG
                //                printf("Vizinho topo: %d(%d-4)\n", fork3, m);
                informarVizinho(fork3, m);
            }
            m++;
        }
    }
    // End adicionar vizinhos
}

void criaProcessosTorus()
{
    int i = 0;
    int pid_fork;
    int contador_no_ref = 0;
    
    /*criando os processos*/
    for (i; i < n_nos_topologia; i++)
    {
        if (getpid() == pid_principal)
        {
            no_ref = contador_no_ref + 1;

            pid_fork = fork();

            if (pid_fork > 0)
            {
                // Aqui os processos sao inseridos na tabela de processos, localizada
                // na global.h
                tab_proc[contador_no_ref].pid = pid_fork;
                tab_proc[contador_no_ref].no_ref = contador_no_ref + 1;
                tab_proc[contador_no_ref].livre = 1;

                // Adição idêntica a adição dos processos da fat tree, mas os meus são numerados de 0 a 15
                // no NO_REF(NoID)
                noTorus[i].pid = pid_fork;

                contador_no_ref++;
            }
        }
        else
        {
            break;
        }
    }
    /* Ou seja, os filhos*/
    if (getpid() != pid_principal)
    {
        printf("Meu pid: %d | Meu ref: %d\n", getpid(), no_ref);
        executaProgramaNosTorus();
        // executa_programa();
    }
    // DEBUG
    else {
        for (i = 0; i < n_nos_topologia; i++)
        {
            printf("tabproc[%d]\tpid: %d\tno_ref:%d\t livre: %d\n", i, tab_proc[i].pid, tab_proc[i].no_ref, tab_proc[i].livre);
            printf("NoTorus[%d]\tpid: %d\tnoId:%d\n", i, noTorus[i].pid, noTorus[i].noId);
        }
        
    //     // aciona_execucao_prog("./a.out", "a");
        
    }
}

/**
 * Nos filhos(0 a 15) recebem a mensagem de execucao e executam o programa em trata_mensagem_torus
 * */
void executaProgramaNosTorus(){
    mensagem msg;
    int valor;
    int i = 0;
    while(1){
        if (msgrcv(msgid_fila_topologia, &msg, sizeof(msg), no_ref, IPC_NOWAIT) < 0) {
            // printf("noRef receive executa: %d\n", no_ref);
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem executa programa\n");

		}
		else {    
            printf("Recebi | mtype: %ld\n", msg.mtype);
            
            if(msg.mtype == 1 && msg.operacao == TYPE_EXEC){
                valor = TYPE_NO_1;
                for(i; i< n_nos_topologia; i++){
                    while(valor != i+1){
                        valor = calculaCaminho(msg, i+1, valor);
                    }
                }
                tempos[0] = clock();
		    	trata_mensagem_torus(msg.mtext, msg.prog);
		    	tempos[1] = clock();
                fflush(stdout);
                fim_programa_torus(tempos[0], tempos[1]);
            }
            // Se o no de destino for o mesmo mtype
            // Então eh pra executar
            if(msg.mtype == msg.no_dest && msg.mtype != TYPE_FIN){ 
                printf("Executarei | mtype: %ld\n", msg.mtype);
                tempos[0] = clock();
		    	trata_mensagem_torus(msg.mtext, msg.prog);
		    	tempos[1] = clock();
                fflush(stdout);
                fim_programa_torus(tempos[0], tempos[1]);                
            }
        }
    }
}

void inicializaTorus()
{
    //  Cria estrutura com a mensagem a ser repassada entre os nós
    mensagem msg;
    int i = 0;

    // Aqui os nos e seus vizinhos sao criados como estruturas de dados
    // sem forks ainda
    criaNosTorus();

    /*guarda o pid do escalonador*/
    pid_principal = getpid();

    /*cria a fila de mensagem para comunicacao*/
    cria_fila_mensagem_torus();
}

int calculaCaminho(mensagem msg, int no_dest, int no_orig)
{
    int type_all = msg.no_dest == TYPE_ALL ? 1 : 0;
    int type_escalonador = msg.no_dest == TYPE_ESC ? 1 : 0;

    
    // Valores absolutos para no de origem e no destino
    int destino = no_dest - 1; // Valores absolutos
    int no_ref = no_orig - 1; // Valores absolutos
    // printf("Destino: %d\nno_ref:%d\n", destino, no_ref);

    int i;
    unsigned int menorvalor;
    int melhorVizinho;

    // Segundo caso, calcula nó específico:
    // Distância é nó de destino - o nó de origem
    // Se o nó destino for o escalonador, isso será negativo...
    // TODO: Fazer o caso de nó voltando para o nó 0 e para o escalonador

    int dist = abs(destino - no_ref);
    //            printf("dist: %d\n", dist);

    // Caso especifico: distancia entre os nos eh 1, mas o no nao eh vizinho do outro
    // ex 8 e 7 ou 7 e 8

    if (dist == 1 && !isVizinho(no_ref, destino))
    {
        //                DEBUG
        //                printf("Nao sao vizinhos\n");
        if ((destino - no_ref) > 0)
        {
            // DEBUG
            // printf(" > 0\n");
            if (isVizinho(no_ref + 4, destino))
            {
                //                            DEBUG
                // printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref, no_ref + 4);
                msg.no_source = no_ref + 1;
                msg.no_dest = destino + 1;
                msg.mtype = no_ref + (4 + 1);
                if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){

                } else {
                    // printf("Mensagem(origem: %d, destino:%d) ENVIADA COM SUCESSO\n",  no_ref, no_ref + 4);
                }  
                // printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref + 4, destino);
                msg.no_source = no_ref + (4 + 1);
                msg.no_dest = destino + 1;
                msg.mtype = destino + 1;
                if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){

                } else {
                    // printf("Mensagem(origem: %d, destino:%d) ENVIADA COM SUCESSO\n",  no_ref + 4, destino);
                }  
                return no_dest;
            }
        }
        else if ((destino - no_ref) < 0)
        {
            // DEBUG 
            // printf(" < 0\n");
            if (isVizinho(no_ref - 4, destino))
            {
                //                        DEBUG
                // printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref, no_ref - 4);
                msg.no_source = no_ref + 1;
                msg.no_dest = destino + 1;
                msg.mtype = no_ref - (4 + 1);
                if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){

                } else {
                    // printf("Mensagem(origem: %d, destino:%d) ENVIADA COM SUCESSO\n",  no_ref, no_ref - 4);
                }                    
                // printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref - 4, destino);
                msg.no_source = no_ref + (4 + 1);
                msg.no_dest = destino + 1;
                msg.mtype = destino + 1;
                if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){

                } else {
                    // printf("Mensagem(origem: %d, destino:%d) ENVIADA COM SUCESSO\n", no_ref - 4, destino);
                }
                return no_dest;
            }
        }
    }
    //        DEBUG
    //             for (i = 0; i < 4; i++) {
    // //                printf("vizinhos: %d\t", noTorus[no_ref].vizinhos[i]);
    //             }
    menorvalor = destino - noTorus[no_ref].vizinhos[0];
    // Calculo o modulo, que eh a distância absoluta entre os nos, independente de ser menor ou maior que 0
    menorvalor = abs(menorvalor);
    //            printf("\nmenorvalor: %d \n", menorvalor);
    melhorVizinho = noTorus[no_ref].vizinhos[0];
    for (i = 1; i < 4; i++)
    {
        if (menorvalor > abs(destino - noTorus[no_ref].vizinhos[i]))
        {
            menorvalor = abs(destino - noTorus[no_ref].vizinhos[i]);
            //                    printf("menorvalor: %d \n", menorvalor);
            melhorVizinho = noTorus[no_ref].vizinhos[i];
        }
    }
    //            printf("Melhor Vizinho: %d\n"1, melhorVizinho);
    msg.no_source = melhorVizinho + 1;
    msg.no_dest = destino + 1;
    msg.mtype = destino + 1;
    // printf("Manda mensagem(origem: %d, destino:%d)\n", no_ref, melhorVizinho);
    if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){

    } else {
        // printf("Mensagem(origem: %d, destino:%d) ENVIADA COM SUCESSO\n", no_ref, melhorVizinho);
    }
    return melhorVizinho + 1;
    // Terceiro caso: quero mandar de volta para o escalonador
}

/**
 * Verifica se o no_ref possui o vizinho no, caso contrário retorna 0
 * */
int isVizinho(int no_ref, int no)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (noTorus[no_ref].vizinhos[i] == no)
        {
            // printf("eh vizinho\n");
            return 1;
        }
    }
    // printf("Nao, no_ref %d nao possui o vizinho %d\n", no_ref, no);
    return 0;
}

/**
 * Cria um fork para realizar a execução do programa enviado por mensagem
 * */

void trata_mensagem_torus(char *caminho_prog, char *programa)
{
    printf("trata_mensagem_torus!!\n");
	pid_t pid_exec;
	int status;

	pid_exec = fork();

	if (pid_exec == 0)
	{
		//filho
      	execl(caminho_prog, programa, NULL);
	//execl("./a.out", "a", NULL);
      	perror("execl() failure!\n\n");

		exit(1);	
	}
	else
	{
		//pai
		wait(&status);
	}
}

/**
 * Envia a mensagem de finalizacao de programa para todos os nos,
 * a recepcao da mensagem tambem ocorre nesse metodo, onde todos os nos
 * encerram sua execucao e retornam o makespan de todos os nos para o no 0
 * */
void fim_programa_torus(unsigned long time_ini, unsigned long time_end)
{
	mensagem msg;
	int contador_roteamento = 0;
    int valor;
    int i = 16;

	msg.mtype  = no_ref;
	msg.pid = pid;
	msg.no_source = no_ref;
	msg.no_dest = TYPE_ESC;
	msg.livre = true;
	msg.operacao = TYPE_FIN;
	msg.time_ini = time_ini;
	msg.time_end = time_end;
	(void) strcpy(msg.mtext,"no terminou exec") ;
	
	if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0) {
	   perror("[CRIANDO]Erro no envio da mensagem") ;
	}


	while(1)
	{
		if (msgrcv(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, no_ref, IPC_NOWAIT) < 0) {
	   		// perror("[TRANSMITE]Erro na recepcao da mensagem fim programa") ;
		}
		else
		{
            fflush(stdout);
			if (msg.operacao == TYPE_FIN){
                if(msg.mtype == TYPE_NO_1 && msg.no_dest == TYPE_NO_1){
                msg.mtype = TYPE_ESC;
                    if(msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){
                        printf("Não mandei mensagem");
                        fflush(stdout);
                    } else {
                        printf("Mandei mensagem | msgid_fila_topologia: %d", msgid_fila_topologia);
                    }
                }
                for(i; i > 0; i--){
                    while(valor != TYPE_NO_1){
                        valor = calculaCaminho(msg, TYPE_NO_1, i+1);
                        if(valor == TYPE_NO_1){
                            msg.mtype = TYPE_ESC;
                            if(msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0){} 
                            else {
                                printf("Mandei mensagem | msgid_fila_topologia: %d", msgid_fila_topologia);
                            }
                        }
                    }
                }
			}
		}
	}
}

/**
 * Envia a mensagem de execucao para o no 1 que repassa para todos os nos
 * atraves do roteamento implementado para a topologia, marca todos os processos
 * como ocupados na tabela de processos
 * */
// void ordem_executa_programa(char *caminho_prog, char *programa)
// {
// 	mensagem msg;
//     printf("ORDEM EXECUTA \n");
//     fflush(stdout);
//     /*
// 	* enviado para o escalonador dados do processo
// 	* notificando que já foi criado e está livre
// 	*/
// 	msg.mtype  = TYPE_NO_1; //coloca type_no_0 quando quer enviar para o escalonador
// 	msg.pid = pid_principal;
// 	msg.no_source = TYPE_ESC; /*TYPE_NO_eu*/
// 	msg.no_dest = TYPE_ALL;
// 	msg.livre = false;
// 	msg.operacao = TYPE_EXEC;
// 	(void) strcpy(msg.mtext,caminho_prog) ;
// 	(void) strcpy(msg.prog,programa) ;

// 	if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0) {
// 	   perror("[ESCALONADOR]Erro no envio da mensagem") ;
// 	}
//     else{
//         printf("Escalonador enviei mensagem\n");
//     }


// 	for (int i = 0; i < n_nos_topologia; ++i)
// 	{
// 		tab_proc[i].livre = 0;
// 	}

// }

/**
 * Essa funcao verifica se todos os nos estao livres, caso sim, ele realiza a
 * comunicacao de execucao entre todos os nos, apos isso espera a resposta para
 * enviar os tempos de 
 * */
// void aciona_execucao_prog(char *caminho_prog, char *programa)
// {
// 	printf("VOU EXECUTAR\n");
// 	if (isNosLivresTorus())
// 	{
// 		printf("EXECUTANDO...\n");
// 		ordem_executa_programa(caminho_prog, programa);

// 		espera_resultado_execucao();
// 	}
// 	else
// 	{
// 		printf("Não posso executar. Nem todos os processos estão livres!\n");
// 	}
// }

int isNosLivresTorus(){
    for (int i = 0; i < n_nos_topologia; ++i)
	{
        // printf("tab_proc[%d].livre: %d\n", i, tab_proc[i].livre);
		if (tab_proc[i].livre == 0)
		{
			return 0;
		}
	}
	return 1;
}

// void espera_resultado_execucao(void)
// {
// 	int contador_no_ref = 0;
// 	int index;
// 	mensagem msg;

// 	while(contador_no_ref < n_nos_topologia)
// 	{
// 		if (msgrcv(msgid, &msg, TAM_TOTAL_MSG, TYPE_ESC, IPC_NOWAIT) < 0) {
// 		//    perror("[ESCALONADOR]Erro na recepcao da mensagem espera_resultado_execucao") ;
// 		}
// 		else
// 		{
// 			if (msg.operacao == TYPE_FIN)
// 			{
// 				index = search_proc(msg.pid, 2);

// 				if (index != -1)
// 				{
// 					printf("[ESCALONADOR]PROCESSO %d finalizou | %lu -> %lu\n", tab_proc[index].no_ref, msg.time_ini, msg.time_end);
//                     fflush(stdout);
// 					marca_gerente_livre(msg.pid, msg.time_ini, msg.time_end);

// 					contador_no_ref++;
// 				}
// 			}
// 		}
// 	}
// }

//Copia de cria_fila_mensagem
//TODO: Remover apos terminar topologia e juntar com a main
// void cria_fila_mensagem(void)
// {
//     mensagem msg;
//     key_fila_msg = KEY;

//     if ((msgid = msgget(key_fila_msg, IPC_CREAT | 0666)) == -1)
//     {
//         perror("Erro de msgget");
//     }
//     else
//     {
//         //avisa ao processo shutdown o pid do escalonador
//         msg.mtype = TYPE_SHUTDOWN;
//         msg.pid = pid_principal;
//         msg.no_source = pid_principal; /*TYPE_NO_eu*/
//         msg.no_dest = 0;
//         msg.operacao = 0;
//         (void)strcpy(msg.mtext, "pid escalonador");

//         if (msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) < 0)
//         {
//             perror("[ESCALONADOR]Erro no envio da mensagem");
//         }
//         ////////
//     }
// }

// int search_proc(int ref, int option)
// {
// 	for (int i = 0; i < n_nos_topologia; ++i)
// 	{
// 		if (option == 1)
// 		{
// 			if (tab_proc[i].no_ref == ref)
// 			{
// 				return i;
// 			}
// 		}
// 		else if(option == 2)
// 		{
// 			if (tab_proc[i].pid == ref)
// 			{
// 				return i;
// 			}

// 		}
// 	}

// 	return -1;
// }

// void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end)
// {
// 	for (int i = 0; i < n_nos_topologia; ++i)
// 	{
// 		if (tab_proc[i].pid == ref)
// 		{
// 			tab_proc[i].livre = 1;
// 			tab_proc[i].time_ini = time_ini;
// 			tab_proc[i].time_end = time_end;
// 		}
// 	}
// }

void cria_fila_mensagem_torus(void)
{
    mensagem msg;
    key_fila_msg = KEY_TORUS;

    if ((msgid_fila_topologia = msgget(key_fila_msg, IPC_CREAT | 0666)) == -1)
    {
        perror("Erro de msgget");
    }
    else
    {
        //avisa ao processo shutdown o pid do escalonador
        msg.mtype = TYPE_SHUTDOWN;
        msg.pid = pid_principal;
        msg.no_source = pid_principal; /*TYPE_NO_eu*/
        msg.no_dest = 0;
        msg.operacao = 0;
        (void)strcpy(msg.mtext, "pid escalonador");

        if (msgsnd(msgid_fila_topologia, &msg, TAM_TOTAL_MSG, 0) < 0)
        {
            perror("[ESCALONADOR]Erro no envio da mensagem");
        }
        ////////
    }
}
