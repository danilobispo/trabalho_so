/*
* Grupo:
* Laís Mendes Gonçalves (110033647)
* Guilherme de Oliveira Silva (110012496)
* Danilo José Bispo Galvão (120114852)
* Helena Schubert I. L. Silva (100012311)
*/
#ifndef _ESCALA_H
#define _ESCALA_H

#include "global.h"


#define KEY_ESC_POSTERGADO 12496
#define KEY_ESC_SHUT 12497

char topologia;
int msgid_postergado;
int msgid_shutdown;
int msgid_fila_topologia;
int programa_executado = 0;

//varáveis globais
int cont_job = 0, menor_job = 0, menor_delay = 10000;
time_t menor_tempo_inicio = 9000000000;
time_t maior_tempo_fim = 0;

void inicia_tab_jobs(void);
void busc_prox_prog (void);
void cria_fila_mensagem_postergado(void);
void recebi_msg_postergado (void);
void espera_execucao_job (void);
void inicializa_topologia_escolhida(void);
void start_hyper_cubo(void);
void start_torus(void);
void start_fat_tree(void);
void ordem_executa_programa(char *caminho_prog, char *programa, int n_nos);
void aciona_execucao_prog(char *caminho_prog, char *programa, int n_nos);
void espera_resultado_execucao(int n_nos);
int search_proc(int ref, int option);
void marca_gerente_livre(int ref, unsigned long time_ini, unsigned long time_end, int n_nos);
int is_todos_livres(int n_nos);
void infos_fila_msg(void);
void cria_fila_shutdown(void);
void the_end(int sig);
void exclui_fila_mensagem(int id_da_fila);





void inicializaTorus(void);
void cria_fila_mensagem_torus(void);


#endif
