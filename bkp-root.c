#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>

//prototipo das funcoes
void rootLoop (Process*);
timedCommand* prepareCommand (char*);
void printMenu (bool);
void checkRun ();
void executeCommand (List*);

//prototipo do tratamento de sinais
void executeScheduled (int);

//listas de comandos declarada globalmente para poder ser acessada no tratamento da interrupcao
List *scheduled = NULL;
List *finished = NULL;

//informacoes declaradas globalmente para estarem disponiveis no tratamento de interrupcoes
Process *rootInfo = NULL;

//variavel declarada globalmente para termos controles de quantos jobs foram realizados
int count = 0;

void rootLoop (Process *info) {
    rootInfo = info;
    //array para receber a entrada do usuario
    char in[256];
    //variavel utilizada para verificar a ocorrencia da interrupcao
    int signalCheck;
    //deifne o tratamento das excecoes
    signal(SIGALRM, executeScheduled);

    while (1) {
        printMenu(false);
        scanf("%[^\n]s", in);
        getchar();

        if (!strcmp(in, "escalonador &") || !strcmp(in, "2")) {
            //loop enquanto o usuario nao pedir para sair
            while (1) {
                printMenu(true);
                //verifica se a interrupcao nao ocorreu aqui
                signalCheck = 0;
                signalCheck = scanf("%[^\n]s", in);

                if (signalCheck == 1) {
                    getchar();
                } else {
                    continue;
                }

                //checa se o usuario quer sair
                if (!strcmp(in, "SAIR")) {
                    break;
                }

                //caso nao queira, a execucao continua abaixo

                //checa se o usuario nao entrou com a forma correta
                if (strstr(in, "executa_postergado") == NULL) {
                    fprintf(stderr, "ERRO: formato desconhecido!\n");
                    fprintf(stderr, "Formato: executa_postergado [delay] [arquivo]\n");
                continue;
                }

                //tratamos a entrada
                timedCommand *command = prepareCommand(in);

                //inserimos o novo elemento na lista
                scheduled = insert(scheduled, command);

                //chama a funcao para ver se tem algo para executar
                checkRun();
            }

            //apaga o alarm caso estivesse setado
            alarm(0);

            //imprimimos para o usuario os comandos nao executados
            if (!isEmpty(scheduled)) {
                cout << "Os seguintes comandos nao foram executados:\n";
                scheduled = show(scheduled);
                //liberamos a memoria que estava alocada
                _delete(scheduled);
                scheduled = NULL;
            }
        } else if (strstr(in, "executa_postergado") != NULL) {
            //prepara o input
            timedCommand *command = prepareCommand(in+strlen("executa_postergado "));
            //executa o comando
            List *newList = (List*) malloc(sizeof(List));
            newList->element = command;
            newList->prox = NULL;
            executeCommand(newList);
        } else if (!strcmp(in, "SAIR")){
            break;
        } else {
            cout << endl << endl << "Desculpe, comando nao reconhecido!" << endl;
        }
    }

    //imprimimos para o usuario os comandos nao executados
    if (!isEmpty(finished)) {
        cout << "\nOs comandos executados foram:" << endl;
        finished = show(finished);
        //liberamos a memoria que estava alocada
        _delete(finished);
        finished = NULL;
    } else {
        cout << "\nNenhum comando foi executado.\n" << endl;
    }
}

//funcao que recupera as informacoes da string de entrada do usuario
//retorna o endereco da nova estrutura que foi criada com as informacoes
timedCommand* prepareCommand (char* in) {
    //cria um novo espaco para armazenar as informacoes novas
    timedCommand *ret = (timedCommand*) malloc(sizeof(timedCommand));
    //guarda a hora em que este comando foi criado
    ret->arrivalTime = time(NULL);
    //seta valores default logo apos a criacao
    ret->argv = NULL;
    ret->startTime = -1;
    ret->argc = 0;
    ret->finishedTime = -1;
    ret->runTime = 0;

    //loop para contar quantas palavras estao presentes na entrada
    for (int i=0; i < strlen(in); i++) {
        //se temos um espaco, outra palavra deve comecar
        if (in[i] == ' ') {
            //loop para ignorar multiplos espacos entrados pelo usuario
            while (++i < strlen(in) && in[i] == ' ');

            //testa se nao esgotamos a string
            if (i < strlen(in)) {
                ret->argc++;
            }
        }
    }

    //se nao temos pelo menos uma palavra de comando (ignorando o delay)
    //dizemos ao usuario que a entrada esta incorreta e encerramos a execucao
    if (ret->argc < 1) {
        cerr << "\nERRO: deve ser passado um tempo e um arquivo!" << endl;
        cerr << "Formato: <seg> <arq_executavel>\n" << endl;
        exit(1);
    }

    //alocamos o espaco necessario para armazenar os comandos
    ret->argv = (char**) malloc(sizeof(char*) * ret->argc+1);
    //definimos o ultimo comando como NULL, para saber onde esta o fim
    ret->argv[ret->argc] = NULL;
    //define qual o numero desse job
    ret->jobNumber = ++count;

    //loop para separar as palavras da entrada em um array de strings
    int currentWord = 0;
    //pegamos o indice de onde comeca os comandos
    int index1 = getDelay(in, ret);
    do {
        //definimos o fim da palavra atual como o comeco
        int index2 = index1;

        //encontramos o fim da palavra atual
        while (index2 < strlen(in)) {
            if (in[index2] == ' ' | in[index2] == '\n') {
                break;
            }
            index2++;
        }

        //alocamos o espaco necessario para a palavra atual
        ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);

        //fazemos a copia, char a char
        int i = 0;

        //verifica se na primeira palavra tem o "./" necessario para execucao
        if (currentWord == 0) {
            if (in[index1] != '.' || in[index1+1] != '/') {
                //alocamos o espaco necessario para a palavra atual
                ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+3);
                ret->argv[currentWord][i++] = '.';
                ret->argv[currentWord][i++] = '/';
            } else {
                //alocamos o espaco necessario para a palavra atual
                ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);
            }
        } else {
            //alocamos o espaco necessario para a palavra atual
            ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);
        }

        for (; index1 < index2; index1++, i++) {
            ret->argv[currentWord][i] = in[index1];
        }

        //definimos o fim da palavra
        ret->argv[currentWord][i] = 0;

        //andamos com o indice de inicio para a proxima palavra, ignorando os espacos
        while (++index1 < strlen(in) && in[index1] == ' ');
    } while (++currentWord < ret->argc);

    //retornamos a nova estrutura criada
    return ret;
}

//recupera o numero de delay informado pelo usuario
//retorna o indice da string de entrada onde o
//comando comeca, ou seja, apos o valor do delay
int getDelay (char* in, timedCommand* command) {
    //faz uma copia da string de entrada
    char num[strlen(in)];
    strcpy(num, in);

    //variavel que sera utilizada para achar o fim da primeira palavra
    int i = 0;

    //acha ate onde vai a primeira palavra (antes do espaco)
    while (i < strlen(in) && in[i] != ' ') {
        i++;
    }

    //troca o espaco por um fim de string
    //para o comando abaixo saber que o
    //o numero esta so na primeira parte
    num[i] = '\0';

    //tem que testar se isso aqui embaixo da certo
    //TESTAR RETORNO DO ATOI

    //converte a primeira palavra em um numero, que eh a quantidade do delay
    //adiciona com o tempo atual, para saber quando este comando deve ser executado
    command->delay = atoi(num);

    //define o tempo esperado de inicio
    command->startTime = command->arrivalTime + command->delay;

    //ignora os espacos ate o primeiro comando
    while (++i < strlen(in) && in[i] == ' ');

    //retorna o indice de onde os comandos comecam
    return i;
}

//funcao para imprimir o menu ao usuario
void printMenu (bool scheduler) {
    if (!scheduler) {
        cout << endl << "MENU PRINCIPAL" << endl << endl;
        cout << "Para sair, entre \"SAIR\"" << endl;
        cout << "Opcao 1: executa_postergado <seg> <arq_executavel>" << endl;
        cout << "Opcao 2: escalonador &" << endl << endl;
        cout << "Informe opcao: ";

    } else {
        cout << endl << "ESCALONADOR" << endl << endl;
        cout << "Para sair, entre \"SAIR\"" << endl;
        cout << "Informe <seg> <arq_executavel>: ";
    }
}

//funcao que trata o sinal do alarm indicando que esta na hora de uma execucao
void executeScheduled (int dummy) {
    //pega o primeiro item da lista dos comandos que estao esperando
    List *current = (List*) malloc(sizeof(List));
    //remove o elemento a ser executado e o retorna na variavel alocada acima
    scheduled = removeFirst(scheduled, current);
    //executa o comando
    executeCommand(current);
    //chemamos se ja esta na hora do proximo comando
    checkRun();
}

//funcao que executa o comando passado como argumento
void executeCommand (List *command) {
    //usado para ignorar o ./ adicionado para execucao
    char *substring = command->element->argv[0] + 2;
    printf("job=%d,arquivo=%s,delay=%d segundos\n\n", command->element->jobNumber, substring, command->element->delay);
    //checa se ainde deve esperar para a execucao
    int diff = command->element->startTime - time(NULL);

    //se precisamos esperar, bloqueamos pelo tempo adequado
    if (diff > 0) {
        sleep(diff);
    }

    //guardamos o tempo real de inicio
    command->element->startTime = time(NULL);
    //executamos o comando
    command->element->runTime = run(command->element->argv);
    //guardamos o tempo de fim
    command->element->finishedTime = time(NULL);
    cout << "\n\nTempo gasto: " << command->element->runTime << endl << endl;

    //o insere na lista dos terminados
    finished = insertEnd(finished, command);
}

//funcao que verifica se existe um comando a executar e
//prepara o sistema para a execucao no tempo correto
void checkRun () {
    //verificamos se existe algum comando para executar agora
    long opt = checkReady(scheduled);

    if (opt > 0) {
        //se ainda falta tempo para a proxima execucao
        //setamos um alarme para a hora correta
        alarm(opt);
    } else if (opt == 0) {
        //se precisamos tratar um agora, resetamos o alarm e chamamos a funcao manualmente
        alarm(0);
        executeScheduled(0);
    } else {
        //caso a lista esteja vazia, resetamos o alarme
        alarm(0);
    }
}

//funcao que recupera as informacoes da string de entrada do usuario
//retorna o endereco da nova estrutura que foi criada com as informacoes
timedCommand* prepareCommand (char* in) {
    //cria um novo espaco para armazenar as informacoes novas
    timedCommand *ret = (timedCommand*) malloc(sizeof(timedCommand));
    //guarda a hora em que este comando foi criado
    ret->arrivalTime = time(NULL);
    //seta valores default logo apos a criacao
    ret->argv = NULL;
    ret->startTime = -1;
    ret->argc = 0;
    ret->finishedTime = -1;
    ret->runTime = 0;

    //loop para contar quantas palavras estao presentes na entrada
    for (int i=0; i < strlen(in); i++) {
        //se temos um espaco, outra palavra deve comecar
        if (in[i] == ' ') {
            //loop para ignorar multiplos espacos entrados pelo usuario
            while (++i < strlen(in) && in[i] == ' ');

            //testa se nao esgotamos a string
            if (i < strlen(in)) {
                ret->argc++;
            }
        }
    }

    //se nao temos pelo menos uma palavra de comando (ignorando o delay)
    //dizemos ao usuario que a entrada esta incorreta e encerramos a execucao
    if (ret->argc < 1) {
        cerr << "\nERRO: deve ser passado um tempo e um arquivo!" << endl;
        cerr << "Formato: <seg> <arq_executavel>\n" << endl;
        exit(1);
    }

    //alocamos o espaco necessario para armazenar os comandos
    ret->argv = (char**) malloc(sizeof(char*) * ret->argc+1);
    //definimos o ultimo comando como NULL, para saber onde esta o fim
    ret->argv[ret->argc] = NULL;
    //define qual o numero desse job
    ret->jobNumber = ++count;

    //loop para separar as palavras da entrada em um array de strings
    int currentWord = 0;
    //pegamos o indice de onde comeca os comandos
    int index1 = getDelay(in, ret);
    do {
        //definimos o fim da palavra atual como o comeco
        int index2 = index1;

        //encontramos o fim da palavra atual
        while (index2 < strlen(in)) {
            if (in[index2] == ' ' | in[index2] == '\n') {
                break;
            }
            index2++;
        }

        //alocamos o espaco necessario para a palavra atual
        ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);

        //fazemos a copia, char a char
        int i = 0;

        //verifica se na primeira palavra tem o "./" necessario para execucao
        if (currentWord == 0) {
            if (in[index1] != '.' || in[index1+1] != '/') {
                //alocamos o espaco necessario para a palavra atual
                ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+3);
                ret->argv[currentWord][i++] = '.';
                ret->argv[currentWord][i++] = '/';
            } else {
                //alocamos o espaco necessario para a palavra atual
                ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);
            }
        } else {
            //alocamos o espaco necessario para a palavra atual
            ret->argv[currentWord] = (char*) malloc(sizeof(char) * index2-index1+1);
        }

        for (; index1 < index2; index1++, i++) {
            ret->argv[currentWord][i] = in[index1];
        }

        //definimos o fim da palavra
        ret->argv[currentWord][i] = 0;

        //andamos com o indice de inicio para a proxima palavra, ignorando os espacos
        while (++index1 < strlen(in) && in[index1] == ' ');
    } while (++currentWord < ret->argc);

    //retornamos a nova estrutura criada
    return ret;
}
