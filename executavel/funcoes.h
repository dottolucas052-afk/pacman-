#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h> 
#include "raylib.h"

typedef struct {
    int linha;
    int coluna;
} tipo_posicao;

typedef enum { CIMA, BAIXO, ESQUERDA, DIREITA } direcao;

typedef enum { PACMAN, FANTASMA } TipoEntidade;

typedef struct {
    TipoEntidade tipo;
    tipo_posicao posicao;
    tipo_posicao posicao_anterior;
    direcao direcao_atual;
    direcao proxima_direcao;
    float velocidade;
    bool andar;
    bool teleportado;
    bool ativo; 
    float timer_respawn;
} tipo_objeto;

// --- Protótipos ---
void mover_para(tipo_objeto *personagem, direcao d);
tipo_posicao verificar(tipo_objeto personagem, direcao d);
direcao direcao_oposta(direcao d);
bool colidiu_com_parede(tipo_objeto personagem, direcao d, char mapa[20][41]);
bool colidiu_com_fantasma(tipo_objeto *personagem, direcao d, tipo_objeto array_fantasmas[], int qnt_f);
int direcoes_livres_avaliar(tipo_objeto personagem, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41], direcao livres[4], bool evitar_reversao);
void mover_fantasma(tipo_objeto *fantasma, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41]);
tipo_posicao checar_teleporte(tipo_objeto *personagem, tipo_posicao portais[], int qnt_portais);
void verificar_colisao_pacman_fantasma(tipo_objeto *pacman, tipo_objeto array_fantasmas[], int qnt_f, bool power_up_ativo, tipo_posicao pos_inicial_pacman, int *pontos_ptr, int *v, char controle[20][41], int nivel, bool *pacman_morrendo);
void salvar_jogo(const char mapa[20][41], int vidas, int pontos, int nivel, int pellets, tipo_objeto pacman, tipo_objeto *fantasmas, int qnt_f, bool power_up_ativo, int power_up_timer);
bool carregar_jogo(char mapa[20][41], int *vidas, int *pontos, int *nivel, int *pellets, tipo_objeto *pacman, tipo_objeto **fantasmas, int *qnt_f, bool *power_up_ativo, int *power_up_timer, const char *nome_arquivo);

// ATUALIZADO: Agora recebe 'controle'
void inicializar_mapa(FILE *arq, int *nivel, tipo_objeto **array_fantasmas, char mapa[20][41], char controle[20][41], int *qnt_portais, int *qnt_f, int *pellets, tipo_objeto *pacman, tipo_posicao *pos_inicial_pacman, tipo_posicao **portais);

// --- Implementações ---

void mover_para(tipo_objeto *personagem, direcao d) {
    switch (d) {
        case CIMA: personagem->posicao.linha--; break;
        case BAIXO: personagem->posicao.linha++; break;
        case ESQUERDA: personagem->posicao.coluna--; break;
        case DIREITA: personagem->posicao.coluna++; break;
    }
}

tipo_posicao verificar(tipo_objeto personagem, direcao d) {
    tipo_posicao p = personagem.posicao;
    switch (d) {
        case CIMA: p.linha--; break;
        case BAIXO: p.linha++; break;
        case ESQUERDA: p.coluna--; break;
        case DIREITA: p.coluna++; break;
    }
    return p;
}

direcao direcao_oposta(direcao d) {
    switch (d) {
        case CIMA: return BAIXO;
        case BAIXO: return CIMA;
        case ESQUERDA: return DIREITA;
        case DIREITA: return ESQUERDA;
    }
    return CIMA; 
}

bool colidiu_com_parede(tipo_objeto personagem, direcao d, char mapa[20][41]) {
    tipo_posicao p = verificar(personagem, d);
    if (p.linha < 0 || p.linha >= 20 || p.coluna < 0 || p.coluna >= 40) return true;
    if (mapa[p.linha][p.coluna]=='#') return true;
    return false;
}

bool colidiu_com_fantasma(tipo_objeto *personagem, direcao d, tipo_objeto array_fantasmas[], int qnt_f) {
    tipo_posicao p = verificar(*personagem, d);
    for (int i = 0; i < qnt_f; i++) {
        if (!array_fantasmas[i].ativo) continue;
        if (array_fantasmas[i].posicao.linha == p.linha && array_fantasmas[i].posicao.coluna == p.coluna){
            if(&array_fantasmas[i]==personagem) continue; 
            return true;
        }
    }
    return false;
}

int direcoes_livres_avaliar(tipo_objeto personagem, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41], direcao livres[4], bool evitar_reversao) {
    int total = 0;
    direcao candidatos[4] = { CIMA, BAIXO, ESQUERDA, DIREITA };
    direcao op = direcao_oposta(personagem.direcao_atual);

    for (int i = 0; i < 4; i++) {
        direcao cand = candidatos[i];
        if (evitar_reversao && cand == op) continue;
        if (!colidiu_com_parede(personagem, cand, mapa) && !colidiu_com_fantasma(&personagem, cand, array_fantasmas, qnt_f)) {
            livres[total++] = cand;
        }
    }
    if (evitar_reversao && total == 0) {
        for (int i = 0; i < 4; i++) {
            direcao cand = candidatos[i];
            if (!colidiu_com_parede(personagem, cand, mapa) && !colidiu_com_fantasma(&personagem, cand, array_fantasmas, qnt_f)) {
                livres[total++] = cand;
            }
        }
    }
    return total;
}

void mover_fantasma(tipo_objeto *fantasma, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41]) {
    direcao livres[4];
    int total_livre;

    if (fantasma->proxima_direcao != fantasma->direcao_atual) {
        if (!colidiu_com_parede(*fantasma, fantasma->proxima_direcao, mapa) &&
            !colidiu_com_fantasma(fantasma, fantasma->proxima_direcao, array_fantasmas, qnt_f)) {
            fantasma->direcao_atual = fantasma->proxima_direcao;
            mover_para(fantasma, fantasma->direcao_atual);
            total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
            if (total_livre > 0) fantasma->proxima_direcao = livres[rand() % total_livre];
            else fantasma->proxima_direcao = direcao_oposta(fantasma->direcao_atual); 
            return;
        }
    }

    if (!colidiu_com_parede(*fantasma, fantasma->direcao_atual, mapa) &&
        !colidiu_com_fantasma(fantasma, fantasma->direcao_atual, array_fantasmas, qnt_f)) {
        mover_para(fantasma, fantasma->direcao_atual);
        return;
    }

    total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
    if (total_livre > 0) {
        fantasma->direcao_atual = livres[rand() % total_livre];
        mover_para(fantasma, fantasma->direcao_atual);
        total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
        if (total_livre > 0) fantasma->proxima_direcao = livres[rand() % total_livre];
        else fantasma->proxima_direcao = direcao_oposta(fantasma->direcao_atual);
    }
}

tipo_posicao checar_teleporte(tipo_objeto *personagem, tipo_posicao portais[], int qnt_portais) {
    for (int i = 0; i < qnt_portais; i++) {
        if (personagem->posicao.linha == portais[i].linha && personagem->posicao.coluna == portais[i].coluna && personagem->teleportado == false) {
            if (portais[i].coluna<=2||portais[i].coluna>=37){
                for (int j =0; j<qnt_portais; j++){
                    if (i != j && portais[i].linha == portais[j].linha){
                        personagem->posicao = portais[j];
                        personagem->teleportado = true;
                        return personagem->posicao;
                    }
                }         
            }
            if (portais[i].linha<=2||portais[i].linha>=17){
                for (int j =0; j<qnt_portais; j++){
                    if (i != j && portais[i].coluna == portais[j].coluna){
                        personagem->posicao = portais[j];
                        personagem->teleportado = true;
                        return personagem->posicao;
                    }
                }         
            }
        }
    }
    return personagem->posicao; 
}

void verificar_colisao_pacman_fantasma(tipo_objeto *pacman, tipo_objeto array_fantasmas[], int qnt_f, bool power_up_ativo, tipo_posicao pos_inicial_pacman, int *pontos_ptr, int *v, char controle[20][41], int nivel, bool *pacman_morrendo) {
    for(int i = 0; i < qnt_f; i++){
        
        if (!array_fantasmas[i].ativo) continue; 

        bool colisao = (pacman->posicao.linha == array_fantasmas[i].posicao.linha && pacman->posicao.coluna == array_fantasmas[i].posicao.coluna);
        bool troca = (pacman->posicao_anterior.linha == array_fantasmas[i].posicao.linha && pacman->posicao_anterior.coluna == array_fantasmas[i].posicao.coluna 
        && array_fantasmas[i].posicao_anterior.linha == pacman->posicao.linha && array_fantasmas[i].posicao_anterior.coluna == pacman->posicao.coluna);
        
        if(troca || colisao){
             if (power_up_ativo) {
                (*pontos_ptr) += 100;
                array_fantasmas[i].ativo = false; 
                array_fantasmas[i].timer_respawn = 5.0f;
                
            } else {
                *pacman_morrendo = true;
                (*pontos_ptr) -= 200;
                if (*pontos_ptr < 0) *pontos_ptr = 0;
            }
        }
    }
}

void salvar_jogo(const char mapa[20][41], int vidas, int pontos, int nivel, int pellets, tipo_objeto pacman, tipo_objeto *fantasmas, int qnt_f, bool power_up_ativo, int power_up_timer) {
    FILE *arq = fopen("saves/save1.txt", "w");
    if (arq == NULL) {
        printf("Erro ao salvar jogo.\n");
        return;
    }
    fprintf(arq, "%d %d %d %d %d %d\n", vidas, pontos, nivel, pellets, power_up_ativo, power_up_timer);
             
    fprintf(arq, "%d %d %d %d %f %d %d\n", pacman.posicao.linha, pacman.posicao.coluna, pacman.direcao_atual, pacman.proxima_direcao, pacman.velocidade, pacman.andar, pacman.teleportado);
    
    fprintf(arq, "%d\n", qnt_f);
    for (int i = 0; i < qnt_f; i++) {
        fprintf(arq, "%d %d %d %f\n", fantasmas[i].posicao.linha, fantasmas[i].posicao.coluna, fantasmas[i].ativo, fantasmas[i].timer_respawn);
    }

    fprintf(arq, "\n");
    for (int j = 0; j < 20; j++) {
        fprintf(arq, "%s\n", mapa[j]);
    }
    
    fprintf(arq, "\n");
    fclose(arq);
    printf("JOGO SALVO COM SUCESSO!\n");
}

bool carregar_jogo(char mapa[20][41], int *vidas, int *pontos, int *nivel, int *pellets, tipo_objeto *pacman, tipo_objeto **fantasmas, int *qnt_f, bool *power_up_ativo, int *power_up_timer, const char *nome_arquivo) {

    FILE *arq = fopen(nome_arquivo, "r");
    if (arq == NULL) {
        printf("ERRO: O arquivo de save '%s' não foi encontrado.\n", nome_arquivo);
        return false;
    }
    
    int temp_power_up_ativo;
    int pacman_direcao_atual, pacman_proxima_direcao;
    int pacman_andar, pacman_teleportado;
    
    if (fscanf(arq, "%d %d %d %d %d %d\n", 
            vidas, pontos, nivel, pellets, 
            &temp_power_up_ativo, power_up_timer) != 6) {
        printf("ERRO: Dados de estado incompletos.\n");
        fclose(arq);
        return false;
    }
    *power_up_ativo = (bool)temp_power_up_ativo;

    if (fscanf(arq, "%d %d %d %d %f %d %d\n", 
            &pacman->posicao.linha, &pacman->posicao.coluna, 
            &pacman_direcao_atual, &pacman_proxima_direcao, 
            &pacman->velocidade, 
            &pacman_andar, &pacman_teleportado) != 7) {
        printf("ERRO: Dados do Pac-Man incompletos.\n");
        fclose(arq);
        return false;
    }

    pacman->direcao_atual = (direcao)pacman_direcao_atual;
    pacman->proxima_direcao = (direcao)pacman_proxima_direcao;
    pacman->andar = (bool)pacman_andar;
    pacman->teleportado = (bool)pacman_teleportado;
    pacman->posicao_anterior = pacman->posicao; 

    int nova_qnt_f;
    if (fscanf(arq, "%d\n", &nova_qnt_f) != 1) {
        printf("ERRO: Quantidade de fantasmas incompleta.\n");
        fclose(arq);
        return false;
    }
    
    if (*fantasmas != NULL) free(*fantasmas);
    *qnt_f = nova_qnt_f;
    if (*qnt_f > 0) {
        *fantasmas = (tipo_objeto*)malloc(*qnt_f * sizeof(tipo_objeto));
    } else {
        *fantasmas = NULL;
    }

    for (int i = 0; i < *qnt_f; i++) {
        int ativo_temp = 1;
        float timer_temp = 0.0f;
        int lidos = fscanf(arq, "%d %d %d %f\n", &(*fantasmas)[i].posicao.linha, &(*fantasmas)[i].posicao.coluna, &ativo_temp, &timer_temp);
        
        if (lidos < 2) {
             printf("ERRO: Posição do fantasma %d incompleta.\n", i);
             fclose(arq);
             return false;
        }

        (*fantasmas)[i].ativo = (bool)ativo_temp;
        (*fantasmas)[i].timer_respawn = timer_temp;
        (*fantasmas)[i].posicao_anterior = (*fantasmas)[i].posicao;
        (*fantasmas)[i].tipo = FANTASMA;
        (*fantasmas)[i].velocidade = 7.0f; 
        (*fantasmas)[i].andar = true; 
        (*fantasmas)[i].teleportado = false;
        (*fantasmas)[i].direcao_atual = CIMA; 
        (*fantasmas)[i].proxima_direcao = CIMA;
    }

    for (int i = 0; i < 20; i++) {
        if (fgets(mapa[i], 41, arq) == NULL) {
             printf("ERRO: Leitura do mapa incompleta na linha %d.\n", i);
             fclose(arq);
             return false;
        }
        mapa[i][40] = '\0'; 
        if (fgetc(arq) != '\n') { }
    }
    
    fclose(arq);
    printf("JOGO CARREGADO COM SUCESSO! Nivel: %d\n", *nivel);
    return true;
}

// CORREÇÃO: Recebe 'controle' e salva os dados lá antes de limpar o 'mapa'
void inicializar_mapa(FILE *arq, int *nivel, tipo_objeto **array_fantasmas, char mapa[20][41], char controle[20][41], int *qnt_portais, int *qnt_f, int *pellets, tipo_objeto *pacman, tipo_posicao *pos_inicial_pacman, tipo_posicao **portais) {
    
    char texto_mapa[20];
    snprintf(texto_mapa, sizeof(texto_mapa), "mapa%d.txt", *nivel);
    
    arq = fopen(texto_mapa, "r");
    if (arq == NULL) {
        printf("ERRO: Nao foi possivel abrir %s\n", texto_mapa);
        return;
    }

    *qnt_f = 0;
    *pellets = 0;
    *qnt_portais = 0;

    if (*array_fantasmas != NULL) free(*array_fantasmas);
    *array_fantasmas = NULL;
    
    if (*portais != NULL) free(*portais);
    *portais = NULL;

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 40; j++) {
            
            int c = fgetc(arq);
            while (c == '\r' || c == '\n') c = fgetc(arq);
            if (c == EOF) c = ' ';
            
            mapa[i][j] = (char)c;
            // Salva na matriz de controle IMEDIATAMENTE, antes de apagar
            controle[i][j] = (char)c;

            if (mapa[i][j] == 'F') {
                (*qnt_f)++; 
                tipo_objeto *temp = realloc(*array_fantasmas, (*qnt_f) * sizeof(tipo_objeto));
                if (temp == NULL) exit(1); 
                *array_fantasmas = temp;

                int idx = (*qnt_f) - 1;
                (*array_fantasmas)[idx].tipo = FANTASMA;
                (*array_fantasmas)[idx].posicao.linha = i;
                (*array_fantasmas)[idx].posicao.coluna = j;
                (*array_fantasmas)[idx].direcao_atual = CIMA;
                (*array_fantasmas)[idx].proxima_direcao = CIMA;
                (*array_fantasmas)[idx].andar = true;
                (*array_fantasmas)[idx].teleportado = false;
                (*array_fantasmas)[idx].velocidade = 7.0f;
                (*array_fantasmas)[idx].ativo = true;
                (*array_fantasmas)[idx].timer_respawn = 0.0f;
                (*array_fantasmas)[idx].posicao_anterior = (*array_fantasmas)[idx].posicao;
                
                mapa[i][j] = ' '; 
            }
            
            else if (mapa[i][j] == 'T') {
                (*qnt_portais)++;
                
                tipo_posicao *temp_p = realloc(*portais, (*qnt_portais) * sizeof(tipo_posicao));
                if (temp_p == NULL) exit(1);
                *portais = temp_p;

                int idx = (*qnt_portais) - 1;
                (*portais)[idx].linha = i;
                (*portais)[idx].coluna = j;
            }
            
            else if (mapa[i][j] == 'P') {
                pacman->posicao.linha = i;
                pacman->posicao.coluna = j;
                pos_inicial_pacman->linha = i;
                pos_inicial_pacman->coluna = j;
                pacman->posicao_anterior = pacman->posicao;
                pacman->andar = false;
                pacman->teleportado = false;
            }
            
            else if(mapa[i][j] == '.'|| mapa[i][j] == 'o'){
                (*pellets)++;
            }
        }
        mapa[i][40] = '\0';
        controle[i][40] = '\0';
    }
    fclose(arq);
}

#endif

