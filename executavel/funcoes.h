#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "raylib.h"
#define max_saves 64


typedef struct {
    int linha;
    int coluna;
} tipo_posicao;


typedef enum { CIMA, BAIXO, ESQUERDA, DIREITA } direcao;

// ----------------------
// Estruturas de entidades
// ----------------------
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
} tipo_objeto;


// ----------------------
// Definição de funções 
// ----------------------
void mover_para(tipo_objeto *personagem, direcao d);
tipo_posicao verificar(tipo_objeto personagem, direcao d);
direcao direcao_oposta(direcao d);
bool colidiu_com_parede(tipo_objeto personagem, direcao d, char mapa[20][41]);
bool colidiu_com_fantasma(tipo_objeto *personagem, direcao d, tipo_objeto array_fantasmas[], int qnt_f);
int direcoes_livres_avaliar(tipo_objeto personagem, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41], direcao livres[4], bool evitar_reversao);
void mover_fantasma(tipo_objeto *fantasma, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41]);


// ----------------------
// Implementação das funções
// ----------------------
void mover_para(tipo_objeto *personagem, direcao d) {
    switch (d) {
        case CIMA: personagem->posicao.linha--; break;
        case BAIXO: personagem->posicao.linha++; break;
        case ESQUERDA: personagem->posicao.coluna--; break;
        case DIREITA: personagem->posicao.coluna++; break;
    }
}

tipo_posicao verificar(tipo_objeto personagem, direcao d) {//função que simula o movimento sem aplicá-lo justamente para verficiar
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
    return CIMA; // fallback
}

bool colidiu_com_parede(tipo_objeto personagem, direcao d, char mapa[20][41]) {
    tipo_posicao p = verificar(personagem, d);
    // checagem de borda para evitar indexação inválida
    if (p.linha < 0 || p.linha >= 20 || p.coluna < 0 || p.coluna >= 40) return true;
    if (mapa[p.linha][p.coluna]=='#') return true;
    return false;
}

bool colidiu_com_fantasma(tipo_objeto *personagem, direcao d, tipo_objeto array_fantasmas[], int qnt_f) {
    tipo_posicao p = verificar(*personagem, d);
    for (int i = 0; i < qnt_f; i++) {
        if (array_fantasmas[i].posicao.linha == p.linha && array_fantasmas[i].posicao.coluna == p.coluna){
            if(&array_fantasmas[i]==personagem) continue; // ignora o próprio fantasma
            return true;
        }
    }
    return false;
}

int direcoes_livres_avaliar(tipo_objeto personagem, tipo_objeto array_fantasmas[], int qnt_f, char mapa[20][41], direcao livres[4], bool evitar_reversao) {
    int total = 0;
    direcao candidatos[4] = { CIMA, BAIXO, ESQUERDA, DIREITA };
    direcao op = direcao_oposta(personagem.direcao_atual);

// ~~~~ tenta sem permitir reversão (se solicitado) ~~~~//
    for (int i = 0; i < 4; i++) {
        direcao cand = candidatos[i];
        if (evitar_reversao && cand == op) continue;
        if (!colidiu_com_parede(personagem, cand, mapa) && !colidiu_com_fantasma(&personagem, cand, array_fantasmas, qnt_f)) {
            livres[total++] = cand;
        }
    }
    // se pediu para evitar reversao mas não encontrou nada, permite reversao
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

 // ~~ 1) tenta proxima_direcao (se diferente da atual) ~~ //
    if (fantasma->proxima_direcao != fantasma->direcao_atual) {
        if (!colidiu_com_parede(*fantasma, fantasma->proxima_direcao, mapa) &&
            !colidiu_com_fantasma(fantasma, fantasma->proxima_direcao, array_fantasmas, qnt_f)) {
            // muda para proxima e anda
            fantasma->direcao_atual = fantasma->proxima_direcao;
            mover_para(fantasma, fantasma->direcao_atual);
            // calcula próximas opções reais (evitando reversao se possível)
            total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
            if (total_livre > 0) fantasma->proxima_direcao = livres[rand() % total_livre];
            else fantasma->proxima_direcao = direcao_oposta(fantasma->direcao_atual); // fallback
            return;
        }
    }
// ~~~~ 2) tenta continuar na direcao_atual ~~~~ //
    if (!colidiu_com_parede(*fantasma, fantasma->direcao_atual, mapa) &&
        !colidiu_com_fantasma(fantasma, fantasma->direcao_atual, array_fantasmas, qnt_f)) {
        mover_para(fantasma, fantasma->direcao_atual);
        return;
    }
// ~~~~ 3) calcula direcoes livres (evitando reversao) ~~~~ //
    total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
    if (total_livre > 0) {
        fantasma->direcao_atual = livres[rand() % total_livre];
        mover_para(fantasma, fantasma->direcao_atual);
        // atualiza proxima_direcao com base nas novas possibilidades
        total_livre = direcoes_livres_avaliar(*fantasma, array_fantasmas, qnt_f, mapa, livres, true);
        if (total_livre > 0) fantasma->proxima_direcao = livres[rand() % total_livre];
        else fantasma->proxima_direcao = direcao_oposta(fantasma->direcao_atual);
    }
// ~~~~ se não houver direções livres, fica parado ~~~~//
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
    return personagem->posicao; // Se não estava em nenhum portal, não muda
}

void verificar_colisao_pacman_fantasma(tipo_objeto *pacman, tipo_objeto array_fantasmas[], int qnt_f, bool power_up_ativo, tipo_posicao pos_inicial_pacman, int *pontos_ptr, int *v) {
    for(int i = 0; i< qnt_f; i++){
        bool colisao = (pacman->posicao.linha == array_fantasmas[i].posicao.linha && pacman->posicao.coluna == array_fantasmas[i].posicao.coluna);
        bool troca = (pacman->posicao_anterior.linha == array_fantasmas[i].posicao.linha && pacman->posicao_anterior.coluna == array_fantasmas[i].posicao.coluna 
        && array_fantasmas[i].posicao_anterior.linha == pacman->posicao.linha && array_fantasmas[i].posicao_anterior.coluna == pacman->posicao.coluna);
        if(troca||colisao){
             if (power_up_ativo) {

                array_fantasmas[i].posicao.linha = 0;
                array_fantasmas[i].posicao.coluna = 0;
                (*pontos_ptr) += 100;
                //Lógica de pontuação 
                
                
            } else {
                pacman->posicao = pos_inicial_pacman;
                (*pontos_ptr) -= 200;
                if (*pontos_ptr < 0) *pontos_ptr = 0;
                *(v) -= 1;
        
                
            }
        }
        
    }
    }

    void encontrar_proximo_nome_save(char *buffer) {
        int numero_save = 1;
        FILE *arq = NULL;

        do {
            
            snprintf(buffer, max_saves, "saves/save%d.txt", numero_save);

    
            arq = fopen(buffer, "r");

            if (arq != NULL) {
                fclose(arq);
                numero_save++;
            }
            
        } while (arq != NULL);

        printf("Salvando como: %s\n", buffer);
    }

    void salvar_jogo(const char mapa[20][41], int vidas, int pontos, int nivel, int pellets,tipo_objeto pacman, tipo_objeto *fantasmas, int qnt_f,bool power_up_ativo, int power_up_timer) {
        
        char nome_arquivo[max_saves];
        encontrar_proximo_nome_save(nome_arquivo);

        FILE *arq = fopen(nome_arquivo, "w");
        if (arq == NULL) {
            printf("Erro ao salvar jogo.\n");
            return;
        }
        fprintf(arq, "%d %d %d %d %d %d\n", vidas, pontos, nivel, pellets, power_up_ativo, power_up_timer);
                 
        fprintf(arq, "%d %d %d %d %f %d %d\n", pacman.posicao.linha, pacman.posicao.coluna, pacman.direcao_atual, pacman.proxima_direcao, pacman.velocidade, pacman.andar, pacman.teleportado);
        
        fprintf(arq, "%d\n", qnt_f);
        for (int i = 0; i < qnt_f; i++) {
            fprintf(arq, "%d %d\n", fantasmas[i].posicao.linha, fantasmas[i].posicao.coluna);
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
            printf("ERRO: Dados de estado (pontuação/vidas/nível) incompletos.\n");
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

            if (fscanf(arq, "%d %d\n", &(*fantasmas)[i].posicao.linha, &(*fantasmas)[i].posicao.coluna) != 2) {
                printf("ERRO: Posição do fantasma %d incompleta.\n", i);
                fclose(arq);
                return false;
            }

            (*fantasmas)[i].posicao_anterior = (*fantasmas)[i].posicao;
            (*fantasmas)[i].tipo = FANTASMA;
            (*fantasmas)[i].velocidade = 7.0f; 
            (*fantasmas)[i].andar = true; 
            (*fantasmas)[i].teleportado = false;
            (*fantasmas)[i].direcao_atual = CIMA; 
            (*fantasmas)[i].proxima_direcao = CIMA;
        }

        for (int i = 0; i < 20; i++) {
        
        // 1. Tenta ler a linha (40 caracteres)
        if (fgets(mapa[i], 41, arq) == NULL) {
             printf("ERRO: Leitura do mapa incompleta na linha %d.\n", i);
             fclose(arq);
             return false;
        }
        
        // 2. Garante que a string está terminada (o que é feito pelo fgets no índice 40)
        mapa[i][40] = '\0'; 

        // 3. DESCARGA O CARACTERE DE QUEBRA DE LINHA (\n) que sobrou no buffer.
        // Isso garante que o próximo fgets comece a ler a linha seguinte do mapa.
        if (fgetc(arq) != '\n') {
            // Se cair aqui, a estrutura do seu arquivo de save está incorreta (linha de mapa não terminou em \n)
            printf("AVISO: Estrutura do arquivo de save inesperada após a linha %d do mapa.\n", i);
        }
    }
    
    fclose(arq);
    printf("JOGO CARREGADO COM SUCESSO! Nível: %d\n", *nivel);
    return true;
}


