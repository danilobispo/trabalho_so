#ifndef _ESCALA_H
#define _ESCALA_H


#include "global.h"

void inicia_tab_jobs(void);
void busc_prox_prog (void);
void cria_fila_mensagem_postergado(void);
void recebi_msg_postergado (void);
void espera_execucao_job (void);

#endif