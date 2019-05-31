#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>


#define KEY 456
#define TAM_MSG 100
//quantos bytes tem na struct da mensagem
//100(vetor)+20(vetor)+4(int)+1(bool)+4(int)+4(int)+4(int)+8(lu)+8(lu)
#define TAM_TOTAL_MSG 153



typedef struct
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
}mensagem;


int main(int argc, char const *argv[])
{
	int msgid;
	mensagem msg;

    //avisa ao processo shutdown o pid do escalonador
		msg.mtype  = TYPE;
		msg.pid = 1;
		msg.no_source = 2; /*TYPE_NO_eu*/
		msg.no_dest = 0;
		msg.operacao = 0;
		(void) strcpy(msg.mtext,"pid escalonador") ;

    msgsnd(msgid, &msg, TAM_TOTAL_MSG, 0) 

	return 0;
}
