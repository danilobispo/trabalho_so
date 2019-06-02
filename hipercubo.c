#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> 
//#include "global.h"
#define N 15
#define TAM_MSG 100

enum types_mensagens { TYPE_NO_1 = 1, TYPE_NO_2, TYPE_NO_3, TYPE_NO_4, TYPE_NO_5, TYPE_NO_6, TYPE_NO_7,
TYPE_NO_8, TYPE_NO_9, TYPE_NO_10, TYPE_NO_11, TYPE_NO_12, TYPE_NO_13, TYPE_NO_14, TYPE_NO_15, TYPE_NO_16, TYPE_ESC, TYPE_ALL,
TYPE_INI, TYPE_EXEC, TYPE_FIN, TYPE_STOP, TYPE_SHUTDOWN, TYPE_ESC_EXEC, TYPE_ESC_JOBS} types_msg;


 struct tb
{
	int pid;
	int no_ref;
    unsigned long time_ini;
    unsigned long time_end;
	bool livre;
}typedef tabela_processos;


 struct mm
{
    long mtype;
    int pid;
    int no_source;
    int no_dest;
    int operacao;
    unsigned long time_ini;
    unsigned long time_end;
    bool livre;
    char mtext[TAM_MSG];
    char prog[20];
}typedef mensagem;



void cria_familia_hipercubo(tabela_processos *,  tabela_processos *, int ,  mensagem *, int *);

void envia_mensagem_hipercubo( tabela_processos *,  mensagem *, int, int *);

void recebe_mensagem_hipercubo( tabela_processos *,  mensagem *, int, int *);

void envia_reverso_mensagem_hipercubo( tabela_processos *,  mensagem *,  mensagem *, int ,  mensagem *);

int main(){
  	pid_t pid; 
	int idfila, idfila_volta, idfila_esc, estado;
	int i;
	int pid_pai = getpid();
	int enviou=0;
	int recebeu=0;
	char executavel[TAM_MSG+20];
	time_t timer;
	//struct filho filhos[N];
	tabela_processos proprio;
	tabela_processos filhos[N+1];
	mensagem mensagem_env, mensagem_rec;
	mensagem tabela[N+1];
	
	//começa pelo pai, então é 0. Os demais terão que atualizar esse valor.
	proprio.no_ref = 0;

   	if ((idfila = msgget(100012311, IPC_CREAT|0x1B6)) < 0)
   	{
     		printf("erro na criacao da fila 1\n");
     		exit(1);
   	}   	
	if ((idfila_volta = msgget(100012312, IPC_CREAT|0x1B6)) < 0)
   	{
     		printf("erro na criacao da fila 2\n");
     		exit(1);
   	}   	
	if ((idfila_esc = msgget(100012313, IPC_CREAT|0x1B6)) < 0)
   	{
     		printf("erro na criacao da fila 3\n");
     		exit(1);
   	}

	while(1){

		if(msgrcv(idfila_esc, &mensagem_rec, sizeof(mensagem_rec), 1, IPC_NOWAIT)<0){
			perror("\nErro no no 1");
			fflush(stdout);
		}else{

			break;
		}
	}

	strcpy(mensagem_env.prog, mensagem_rec.prog);
	strcpy(mensagem_env.mtext, mensagem_rec.mtext);

	cria_familia_hipercubo(filhos, &proprio, idfila, &mensagem_env,  &pid_pai);
	
	i=0;
	while(recebeu==0){
		//só envia uma vez
		if(enviou==0)
			envia_mensagem_hipercubo(&proprio, &mensagem_env, idfila, &enviou);
		//o loop continua enquanto não tiver recebido
		recebe_mensagem_hipercubo(&proprio, &mensagem_rec, idfila, &recebeu);
		if(recebeu==1)
			strcpy(mensagem_rec.mtext, mensagem_env.mtext);
	}

	strcpy(executavel,mensagem_env.mtext);
	printf("\n[HIPER]CAMINHO = %s\n", executavel);
	fflush(stdout);

//	strcat(executavel, mensagem_env.prog);
	time(&timer);
	pid=fork();
	mensagem_env.time_ini = time(NULL);
	tabela[0].time_ini=mensagem_env.time_ini;
	if(pid==0){
		execl(executavel, "a", NULL);
		//execl("./a.out", "a", NULL);
      		perror("execl() failure!\n\n");

	}else{
		wait(&estado);
	}
	time(&timer);
	mensagem_env.time_end = time(NULL);
	tabela[0].time_end=mensagem_env.time_end;


	envia_reverso_mensagem_hipercubo(&proprio, &mensagem_env, &mensagem_rec, idfila_volta, &tabela);
	
	//printf("pid pai no if %ul\n",tabela[2].time_ini;
	//fflush(stdout);

	if(proprio.no_ref==0){
  		mensagem_env.time_ini=tabela[0].time_ini;
  		mensagem_env.time_end=tabela[0].time_end;
 		mensagem_env.mtype=TYPE_ESC;
		printf("[HIPER]mtype MENSAGEM = %d", mensagem_env.mtype);
		fflush(stdout);
   		printf("\nno %d levou: %lu  -  %lu", i, tabela[i].time_end, (tabela[0].time_end - tabela[0].time_ini));
		msgsnd(idfila_esc, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);
		for(i=0; i<=N; i++){
			printf("\nno %d levou: %lu  -  %lu", i, tabela[i].time_end, (tabela[i].time_end - tabela[i].time_ini));
			mensagem_env.time_ini=tabela[i].time_ini;
			mensagem_env.time_end=tabela[i].time_end;
   			msgsnd(idfila_esc, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);
			
 		}
		printf("\n");

		printf("[HIPER]ID FILA = %d", idfila_esc);
		fflush(stdout);

		sleep(30);
		if(msgctl(idfila, IPC_RMID, &mensagem_env) != 0)
			printf("erro na exclusao mensagem_env\n");
		else
			printf("-----tudo certo na exclusao mensagem_env\n");

		if(msgctl(idfila_volta, IPC_RMID, &mensagem_env) != 0)
			printf("erro na exclusao mensagem_env\n");
		else
			printf("-----tudo certo na exclusao mensagem_env\n");

	}
	
	exit(0);
	return 0;}

void cria_familia_hipercubo( tabela_processos *filho,  tabela_processos *proprio, int idfila,  mensagem *mensagem_env, int *pid_pai){
	int i;
	pid_t pid;

	for(i=0;i<N;i++){
		if( (pid=fork())==0 ){
			*pid_pai=0;
			//printf("sou o filho %d\n", i);
			//kkkk enfim, é aqui que vc diz quem vc é
			proprio->no_ref = i+1;
			mensagem_env->pid = getpid();
			mensagem_env->mtype = i+1;
			//sleep(i+1);
  			//strcpy(mensagem_env->mtext, "teste de mensagem\0");
			//mensagem_env.no = i;
   			//msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
			//printf("criou pid %d, no: %d\n", getpid(), proprio->no_ref);
			break;
		}
		else{
			filho[i].pid=pid;
			filho[i].no_ref=i;
			//printf("sou o pai\n");
		}
	}

}

void envia_mensagem_hipercubo( tabela_processos *proprio,  mensagem *mensagem_env, int idfila, int *enviou){
		
	if(proprio->no_ref==0){
		//o famoso nó 0 manda a mensagem para os nós 1, 2,4 e 8
  		strcpy(mensagem_env->mtext, "teste no 0\0");
		mensagem_env->mtype=1;
		mensagem_env->no_dest = 3;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=2;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=4;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=8;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}

	if(proprio->no_ref==1){
		//nó 1 manda para 5 e 9
		mensagem_env->mtype=5;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=9;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}
	if(proprio->no_ref==2){
		//nó 2 manda mensagem para 3, 6 e 10
		mensagem_env->mtype=3;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=6;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=10;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}
	if(proprio->no_ref==4){
		//nó 4 manda para 12		
		mensagem_env->mtype=12;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}
	if(proprio->no_ref==9){
		//nó 9 manda para 13		
		mensagem_env->mtype=13;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}
	if(proprio->no_ref==3){
		//nó 3 manda para 7 e 11	
		mensagem_env->mtype=7;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		mensagem_env->mtype=11;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);	
		*enviou=1;
	}
	if(proprio->no_ref==7){
		//no 7 manda para 15
		mensagem_env->mtype=15;
		mensagem_env->no_dest = 3;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		*enviou=1;
	}
	if(proprio->no_ref==6){
		//finalmente, 6 manda para 14
		mensagem_env->mtype=14;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);	
		*enviou=1;
	}

}

void recebe_mensagem_hipercubo( tabela_processos *proprio,  mensagem *mensagem_rec, int idfila, int *recebeu){

	//printf("o que ha no proprio->ref de cada um: %d\n", proprio->no_ref);
	switch (proprio->no_ref){
		case 0:
			//msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, IPC_NOWAIT);	
			*recebeu=1;
			break;	
		case 1:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1, IPC_NOWAIT)<0){
				//perror("Erro no no 1");
			}else{
				*recebeu=1;}
			break;
		case 2:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 2, IPC_NOWAIT)<0){
				//perror("Erro no no 2");
			}else{
				*recebeu=1;}
			break;	
		case 3:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 3, IPC_NOWAIT)<0){
				//perror("Erro no no 3");
			}else{
				*recebeu=1;}
			break;	
		case 4:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 4, IPC_NOWAIT)<0){
				//perror("Erro no no 4");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 4 recebu: %s\n", mensagem_rec->mtext);	
			break;	
		case 5:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 5, IPC_NOWAIT)<0){
				//perror("Erro no no 5");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 5 recebu: %s\n", mensagem_rec->mtext);
			break;	
		case 6:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 6, IPC_NOWAIT)<0){
				//perror("Erro no no 6");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 6 recebu: %s\n", mensagem_rec->mtext);	
			break;	
		case 7:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 7, IPC_NOWAIT)<0){
				//perror("Erro no no 7");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 7 recebu: %s\n", mensagem_rec->mtext);
			break;	
		case 8:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 8, IPC_NOWAIT)<0){
				//perror("Erro no no 8");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 8 recebu: %s\n", mensagem_rec->mtext);
			break;	
		case 9:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 9, IPC_NOWAIT)<0){
				//perror("Erro no no 9");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 9 recebu: %s\n", mensagem_rec->mtext);	
			break;	
		case 10:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 10, IPC_NOWAIT)<0){
				//perror("Erro no no 10");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 10 recebu: %s\n", mensagem_rec->mtext);	
			break;	
		case 11:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 11, IPC_NOWAIT)<0){
				//perror("Erro no no 11");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 11 recebu: %s\n", mensagem_rec->mtext);
			break;	
		case 12:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 12, IPC_NOWAIT)<0){
				//perror("Erro no no 12");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 12 recebu: %s\n", mensagem_rec->mtext);}
			break;	
		case 13:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 13, IPC_NOWAIT)<0){
				//perror("Erro no no 13");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 13 recebu: %s\n", mensagem_rec->mtext);}
			break;	
		case 14:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 14, IPC_NOWAIT)<0){
				//perror("Erro no no 14");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 14 recebu: %s\n", mensagem_rec->mtext);}
			break;	
		case 15:
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 15, IPC_NOWAIT)<0){
				//perror("Erro no no 15");
			}else{
				*recebeu=1;}
			//printf("!!!!!!!!!!!!!!!!!no 1 recebu: %s, time ini: %d\n", mensagem_rec->mtext, mensagem_rec->no_dest);}
			break;

	}
}

void envia_reverso_mensagem_hipercubo( tabela_processos *proprio,  mensagem *mensagem_env,  mensagem *mensagem_rec, int idfila,  mensagem *tabela){

	int i=0;
	int recebeu_volta=0;

		
	if(proprio->no_ref==15){
		//15 manda de volta para 7
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=15;
   		if(msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0)<0){
			  	perror("erro no send 15!!!!!!!!!!!!-------------");
				printf("no erro no 15\n");
				fflush(stdout);
		   }else{
			   	printf("no 15 se mandou para cima\n");
				fflush(stdout);
		   }
		
	}

	if(proprio->no_ref==7){
		//no 7 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=7;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		    if(msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0)<0){
			  	perror("erro no send 15!!!!!!!!!!!!-------------");
				printf("no erro no 15\n");
				fflush(stdout);
		   }else{
			   	printf("no 15 se mandou para cima\n");
				fflush(stdout);
		   }
		//no 7 diz quantos nós vai receber
		recebeu_volta=1;
		while(i<recebeu_volta){
			//no 7 tá esperando nó 15
			sleep(1);
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 15, IPC_NOWAIT)<0){
				perror("Erro no no 7 retorno");
			}else{
				strcpy(mensagem_env->mtext, mensagem_rec->mtext);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
				mensagem_env->mtype=(15*10);
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				printf("7 mandou 15 para cima\n");
				fflush(stdout);
				i++;
			}
		}

	}

	if(proprio->no_ref==11){
		//11 manda de volta para 3
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=11;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}


	if(proprio->no_ref==3){
		//no 3 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=3;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 3 diz quantos nós vai receber (três, que são 11, 7 e 15(*10))
		recebeu_volta=3;
		while(i<recebeu_volta){
			//no 3 tá esperando nó 11
			sleep(2);
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 11, IPC_NOWAIT)<0){
				//perror("Erro no no 3 retorno");
			}else{
				//multiplica por 10 o mtype
				mensagem_env->mtype=(11*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 3 mandou 11 pra cima\n");
			}
			//no 3 tá esperando nó 7
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 7, IPC_NOWAIT)<0){
				//perror("Erro no no 3 retorno");
			}else{
				mensagem_env->mtype=(7*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 3 mandou 7 pra cima\n");
			}
			//no 3 tá esperando nó 15(*10)
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 150, IPC_NOWAIT)<0){
				//perror("Erro no no 3 retorno");
			}else{
				mensagem_env->mtype=(15*100);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 3 mandou 15(*10) pra cima\n");
			}
		}

	}

	if(proprio->no_ref==14){
		//14 manda de volta para 6
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=14;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}

	if(proprio->no_ref==6){
		//no 6 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=6;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 6 diz quantos nós vai receber
		recebeu_volta=1;
		while(i<recebeu_volta){
			//no 6 tá esperando nó 14
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 14, IPC_NOWAIT)<0){
				//perror("Erro no no 6 retorno");
			}else{
				mensagem_env->mtype=(14*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 6 mandou 14 pra cima\n");
			}
		}

	}

	if(proprio->no_ref==10){
		//10 manda de volta para 2
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=10;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}


	if(proprio->no_ref==2){
		//no 2 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=2;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 2 diz quantos nós vai receber
		recebeu_volta=7;sleep(1);
		while(i<recebeu_volta){
			//no 2 tá esperando nó 15
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1500, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(15000);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 15 pra cima\n");
			}			
			//no 2 tá esperando nó 7
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 70, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(7*100);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 7 pra cima\n");
			}			
			//no 2 tá esperando nó 11
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 110, IPC_NOWAIT)<0){
				//perror("Erro no no 6 retorno");
			}else{
				mensagem_env->mtype=(11*100);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 11 pra cima\n");
			}
			//no 2 tá esperando nó 14
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 140, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(14*100);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 14 pra cima\n");
			}			
			//no 2 tá esperando nó 3
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 3, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(3*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 3 pra cima\n");
			}			
			//no 2 tá esperando nó 6
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 6, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(6*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 6 pra cima\n");
			}			
			//no 2 tá esperando nó 10
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 10, IPC_NOWAIT)<0){
				//perror("Erro no no 2 retorno");
			}else{
				mensagem_env->mtype=(10*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 2 mandou 10 pra cima\n");
			}
		}

	}

	if(proprio->no_ref==13){
		//13 manda de volta para 9
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=13;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}

	if(proprio->no_ref==5){
		//5 manda de volta para 1
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=5;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}


	if(proprio->no_ref==9){
		//no 9 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=9;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 9 diz quantos nós vai receber
		recebeu_volta=1;
		while(i<recebeu_volta){
			//no 9 tá esperando nó 13
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 13, IPC_NOWAIT)<0){
				//perror("Erro no no 9 retorno");
			}else{
				mensagem_env->mtype=(13*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 9 mandou 13 pra cima\n");
			}
		}

	}

	if(proprio->no_ref==1){
		//no 1 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=1;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 1 diz quantos nós vai receber (três, que são 5, 9 e 13(*10))
		recebeu_volta=3;sleep(5);
		while(i<recebeu_volta){
			//no 1 tá esperando nó 5
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 5, IPC_NOWAIT)<0){
				//perror("Erro no no 1 retorno");
			}else{
				//multiplica por 10 o mtype
				mensagem_env->mtype=(5*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 1 mandou 5 pra cima\n");
			}
			//no 1 tá esperando nó 9
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 9, IPC_NOWAIT)<0){
				//perror("Erro no no 1 retorno");
			}else{
				mensagem_env->mtype=(9*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 1 mandou 9 pra cima\n");
			}
			//no 1 tá esperando nó 13(*10)
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 130, IPC_NOWAIT)<0){
				//perror("Erro no no 1 retorno");
			}else{
				mensagem_env->mtype=(13*100);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				printf("no 1 mandou 13(*100) pra cima\n");
			}
		}

	}

	if(proprio->no_ref==12){
		//12 manda de volta para 4
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=12;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
	}


	if(proprio->no_ref==4){
		//no 4 manda estado de si mesmo
  		strcpy(mensagem_env->mtext, "retorno\0");
		mensagem_env->mtype=4;
   		msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
		//no 4 diz quantos nós vai receber
		recebeu_volta=1;
		while(i<recebeu_volta){
			//no 4 tá esperando nó 12
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 12, IPC_NOWAIT)<0){
				//perror("Erro no no 4 retorno");
			}else{
				mensagem_env->mtype=(12*10);
				mensagem_env->time_ini=mensagem_rec->time_ini;
				mensagem_env->time_end=mensagem_rec->time_end;
   				msgsnd(idfila, mensagem_env, sizeof(*mensagem_env)-sizeof(long), 0);
				i++;
				//printf("no 4 mandou 12 pra cima\n");
			}
		}

	}

	if(proprio->no_ref==8){
		//8 manda de volta para 0
  		strcpy(mensagem_env->mtext, "retorno\0");
		//aqui o mtype é o numero de si mesmo, que é multiplicado por 10 a cada nível
		mensagem_env->mtype=8;
   		if(msgsnd(idfila, mensagem_env, sizeof(*mensagem_env), 0)<0){
			perror("no 8 deu erro\n");
			fflush(stdout);
		}else
		{
			printf("no 8 se mandou pra cima\n");
			fflush(stdout);
			sleep(4);
		}
		
	}

	if(proprio->no_ref==0){
		//no 0 recebe de todo mundo, mas não manda nada (por enquanto)
		//no 0 diz quantos nós vai receber
		recebeu_volta=15;sleep(10);
		while(i<recebeu_volta){
			//no 0 tá esperando nó 15
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 15000, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (15)");
			}else{
  				tabela[15].time_ini=mensagem_rec->time_ini;
  				tabela[15].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 15: %lu, text: %s, time_ini: %lu, time_end: %lu\n", tabela[15].time_ini, mensagem_rec->mtext, mensagem_rec->time_ini, mensagem_rec->time_end);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 700, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (7)");
			}else{
  				tabela[7].time_ini=mensagem_rec->time_ini;
  				tabela[7].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 7: %lu\n", tabela[7].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1100, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (11)");
			}else{
  				tabela[11].time_ini=mensagem_rec->time_ini;
  				tabela[11].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 11: %lu\n", tabela[11].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1300, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (13)");
			}else{
  				tabela[13].time_ini=mensagem_rec->time_ini;
  				tabela[13].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 13: %lu\n", tabela[13].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1400, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (14)");
			}else{
  				tabela[14].time_ini=mensagem_rec->time_ini;
  				tabela[14].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 14: %lu\n", tabela[14].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 30, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (3)");
			}else{
  				tabela[3].time_ini=mensagem_rec->time_ini;
  				tabela[3].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 3: %lu\n", tabela[3].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 50, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (5)");
			}else{
  				tabela[5].time_ini=mensagem_rec->time_ini;
  				tabela[5].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 5: %lu\n", tabela[5].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 60, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (6)");
			}else{
  				tabela[6].time_ini=mensagem_rec->time_ini;
  				tabela[6].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 6: %lu\n", tabela[6].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 90, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (9)");
			}else{
  				tabela[9].time_ini=mensagem_rec->time_ini;
  				tabela[9].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 9: %lu\n", tabela[9].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 100, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (10)");
			}else{
  				tabela[10].time_ini=mensagem_rec->time_ini;
  				tabela[10].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 10: %lu\n", tabela[10].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 120, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (12)");
			}else{
  				tabela[12].time_ini=mensagem_rec->time_ini;
  				tabela[12].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 12: %lu\n", tabela[12].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 1, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (1)");
			}else{
  				tabela[1].time_ini=mensagem_rec->time_ini;
  				tabela[1].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 1: %lu\n", tabela[1].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 2, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (2)");
			}else{
  				tabela[2].time_ini=mensagem_rec->time_ini;
  				tabela[2].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 2: %lu\n", tabela[2].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 4, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (4)");
			}else{
  				tabela[4].time_ini=mensagem_rec->time_ini;
  				tabela[4].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 4: %lu\n", tabela[4].time_ini);
			}			
			if(msgrcv(idfila, mensagem_rec, sizeof(*mensagem_rec), 8, IPC_NOWAIT)<0){
				//perror("Erro no no 0 retorno (8)");
			}else{
  				tabela[8].time_ini=mensagem_rec->time_ini;
  				tabela[8].time_end=mensagem_rec->time_end;
				i++;
				printf("no 0, time_ini 8: %lu\n", tabela[8].time_ini);
			}
		}

	}

}






















