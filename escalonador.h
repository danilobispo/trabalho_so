#ifndef _ESCALA_H
#define _ESCALA_H

#include "global.h"


#define KEY_ESC_POSTERGADO 12496

char topologia;
int msgid_postergado;
int msgid_fila_topologia;
int programa_executado = 0;

//varáveis globais
int cont_job = 0, menor_job = 0, menor_delay = 10000;
time_t menor_tempo_inicio = 0;
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


#endif