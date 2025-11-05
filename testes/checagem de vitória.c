#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "raylib.h"

// ----------------------
// Tipos básicos
// ----------------------
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
            for (int j =0; j<qnt_portais; j++){
                if (i != j && portais[i].linha == portais[j].linha){
                    personagem->posicao = portais[j];
                    personagem->teleportado = true;
                    return personagem->posicao;
                }
            }         
        }
    }
    return personagem->posicao; // Se não estava em nenhum portal, não muda
}

void verificar_colisao_pacman_fantasma(tipo_objeto *pacman, tipo_objeto array_fantasmas[], int qnt_f, bool power_up_ativo, tipo_posicao pos_inicial_pacman) {
    for(int i = 0; i< qnt_f; i++){
        bool colisao = (pacman->posicao.linha == array_fantasmas[i].posicao.linha && pacman->posicao.coluna == array_fantasmas[i].posicao.coluna);
        bool troca = (pacman->posicao_anterior.linha == array_fantasmas[i].posicao.linha && pacman->posicao_anterior.coluna == array_fantasmas[i].posicao.coluna 
        && array_fantasmas[i].posicao_anterior.linha == pacman->posicao.linha && array_fantasmas[i].posicao_anterior.coluna == pacman->posicao.coluna);
        if(troca||colisao){
             if (power_up_ativo) {
                array_fantasmas[i].posicao.linha = 0;
                array_fantasmas[i].posicao.coluna = 0;
                
                //Lógica de pontuação 
                
                
            } else {
                pacman->posicao = pos_inicial_pacman;
                
            }
        }
        
    }
}

int main() {

    // ~~~~ 1) Leitura do arquivo ~~~~ //
    FILE *arq = fopen("mapa.txt", "r");
    if (arq == NULL) {
        printf("ERRO NA ABERTURA DO ARQUIVO\n");
        return 1;
    }

    char mapa[20][41];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 40; j++) {
            int c = fgetc(arq);
            while (c == '\r' || c == '\n') c = fgetc(arq); // ignora quebra de linha
            if (c == EOF) c = ' ';
            mapa[i][j] = (char)c;
        }
        mapa[i][40] = '\0';
    }
    fclose(arq);

    // ~~~~ 2) Inicializações ~~~~ //
    tipo_objeto *array_fantasmas = NULL;
    int qnt_f = 0;
    int pellets = 0;
    tipo_objeto pacman;
    tipo_posicao pos_inicial_pacman;
    //tonalidades de cor
    Color AZUL_ESCURO = (Color){ 0, 40, 100, 255 };   
    Color AZUL_NOITE      = (Color){ 0, 20, 60, 255 };    
    Color AZUL_MARINHO    = (Color){ 0, 0, 80, 255 };     
    Color ROXO_ESCURO = (Color){ 40, 0, 100, 255 };

    // Conta portais (T)
    int contadorT = 0;
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 40; j++)
            if (mapa[i][j] == 'T') contadorT++;

    tipo_posicao *portais = NULL;
    int qnt_portais = 0;
    if (contadorT > 0) portais = malloc(contadorT * sizeof(tipo_posicao));

    // ~~~~ 3) Varre mapa e configura entidades ~~~~ //
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 40; j++) {
            if (mapa[i][j] == 'F') {
                qnt_f++;
                array_fantasmas = realloc(array_fantasmas, qnt_f * sizeof(tipo_objeto));
                array_fantasmas[qnt_f-1].tipo = FANTASMA;
                array_fantasmas[qnt_f-1].posicao.linha = i;
                array_fantasmas[qnt_f-1].posicao.coluna = j;
                array_fantasmas[qnt_f-1].direcao_atual = CIMA;
                array_fantasmas[qnt_f-1].proxima_direcao = CIMA;
                array_fantasmas[qnt_f-1].andar = true;
                array_fantasmas[qnt_f-1].teleportado = false;
                mapa[i][j] = ' ';
            }
            if (mapa[i][j] == 'T') {
                portais[qnt_portais].linha = i;
                portais[qnt_portais].coluna = j;
                qnt_portais++;
            }
            if (mapa[i][j] == 'P') {
                pacman.posicao.linha = i;
                pacman.posicao.coluna = j;

                pos_inicial_pacman.linha = i;
                pos_inicial_pacman.coluna = j;
                pacman.posicao_anterior = pacman.posicao;
                pacman.andar = false;
                pacman.teleportado = false;                           
            }
            if(mapa[i][j] == '.' || mapa[i][j] == 'o'){
                pellets++;
            }
        }
    }

    // ~~~~ 4) Inicializa Raylib ~~~~ //
    const int CELULA = 20;
    const int LARGURA = 40 * CELULA;
    const int ALTURA = 20 * CELULA;
    InitWindow(LARGURA, ALTURA, "Pacman");
    srand(time(NULL));
    SetTargetFPS(60);
    double tempoInicio = 0.0;
    double tempoAtual;
    
    // intervalo entre um desenho e outro
    const float intervalo = 1.0f / 10.0f;
    float contador_tempo = 0.0f;
    
    Vector2 pacman_pos_visual = { pacman.posicao.coluna * CELULA, pacman.posicao.linha * CELULA };
    const float VELOCIDADE_PACMAN = 100.0f;
    
    bool venceu = false;
    
    // Variáveis do Power-Up
    bool power_up_ativo = false;
    int power_up_timer = 0;
    const int TEMPO_POWER_UP = 80; // considerei 8 segundos (ver esquema de velocidade para poder aumentar fps)



    // ~~~~ 5) Loop principal ~~~~ //
    while (!WindowShouldClose()) {
        
        contador_tempo = contador_tempo + GetFrameTime();
        float deslize = contador_tempo / intervalo;
        if(deslize > 1.0f){
            deslize = 1.0f;
        }

        BeginDrawing();
        ClearBackground(AZUL_ESCURO);
        if(venceu){
            tempoAtual = GetTime();
            ClearBackground(WHITE);
            DrawText("Você Venceu!", 250, 200, 40, RED);
            
            
            if(tempoAtual - tempoInicio > 8.0){
                EndDrawing();
                break;
            }
            EndDrawing();
            continue;
        }
        
        int x = pacman.posicao.linha;
        int y = pacman.posicao.coluna;
        
        
        // --- Desenho do Mapa ---
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 40; j++) {
                int px = j * CELULA;
                int py = i * CELULA;
                switch (mapa[i][j]) {
                    case '#': DrawRectangle(px, py, CELULA, CELULA, BLACK); break;
                    case '.': DrawCircle(px + CELULA/2, py + CELULA/2, 3, YELLOW);break;
                    case 'o': DrawCircle(px + CELULA/2, py + CELULA/2, 6, GREEN); break;
                    case 'T': DrawCircle(px + CELULA/2, py + CELULA/2, 5, ORANGE); break;
                }
            }
        }


        Color cor_fantasma = power_up_ativo ? BLUE : PURPLE; //cor vai depender do estado do fantasma
        
        for (int i = 0; i < qnt_f; i++) {
            
            float aux_fantasma_x, aux_fantasma_y;
            
            if (array_fantasmas[i].teleportado) {
                aux_fantasma_x = array_fantasmas[i].posicao.coluna * CELULA;
                aux_fantasma_y = array_fantasmas[i].posicao.linha * CELULA;
            } else {
                aux_fantasma_x = (array_fantasmas[i].posicao_anterior.coluna * (1.0f - deslize) + array_fantasmas[i].posicao.coluna * deslize) * CELULA;
                aux_fantasma_y = (array_fantasmas[i].posicao_anterior.linha * (1.0f - deslize) + array_fantasmas[i].posicao.linha * deslize) * CELULA;
            }

            DrawRectangle(aux_fantasma_x, aux_fantasma_y, CELULA, CELULA, cor_fantasma);
        }
        

        // --- Desenha Pacman ---
        float aux_pacman_x, aux_pacman_y;

        if (pacman.teleportado) {
            aux_pacman_x = pacman.posicao.coluna * CELULA;
            aux_pacman_y = pacman.posicao.linha * CELULA;
        } else {
            aux_pacman_x = (pacman.posicao_anterior.coluna * (1.0f - deslize) + pacman.posicao.coluna * deslize) * CELULA;
            aux_pacman_y = (pacman.posicao_anterior.linha * (1.0f - deslize) + pacman.posicao.linha * deslize) * CELULA;
        }
        DrawCircle(aux_pacman_x + CELULA / 2, aux_pacman_y + CELULA / 2, 9, GOLD);
        EndDrawing();
        

        if(contador_tempo >= intervalo){
            contador_tempo -= intervalo;
            if (mapa[pacman.posicao.linha][pacman.posicao.coluna] != 'T') {
                 pacman.teleportado = false;
            }
            
            for (int i = 0; i < qnt_f; i++) {
                if (mapa[array_fantasmas[i].posicao.linha][array_fantasmas[i].posicao.coluna] != 'T') {
                    array_fantasmas[i].teleportado = false;
                }
            }

            pacman.posicao_anterior = pacman.posicao;
            for(int i = 0; i< qnt_f; i++){
                array_fantasmas[i].posicao_anterior = array_fantasmas[i].posicao;
            }
            if (power_up_ativo) {
                power_up_timer--;
                if (power_up_timer <= 0) {
                    power_up_ativo = false;
                }
            }
            pacman.andar = false;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {pacman.direcao_atual = CIMA; pacman.andar = true;} 
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {pacman.direcao_atual = BAIXO; pacman.andar = true;} 
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {pacman.direcao_atual = DIREITA; pacman.andar = true;} 
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {pacman.direcao_atual = ESQUERDA; pacman.andar = true;}
            
            if (pacman.andar == true){
                pacman.teleportado = false;
                if (!colidiu_com_parede(pacman, pacman.direcao_atual, mapa))
                mover_para(&pacman, pacman.direcao_atual);
            }
            int x = pacman.posicao.linha;
            int y = pacman.posicao.coluna;

            if (mapa[x][y] == '.') {
                pellets--;
                mapa[x][y] = ' ';
                if(pellets == 0 && !venceu){
                    venceu = true;
                    tempoInicio = GetTime();
                } 
            } else if (mapa[x][y] == 'o') {
                pellets--;
                mapa[x][y] = ' ';
                power_up_ativo = true;
                power_up_timer = TEMPO_POWER_UP;
                if(pellets == 0 && !venceu){
                    venceu = true;
                    tempoInicio = GetTime();
                }
            }
            
            checar_teleporte(&pacman, portais, qnt_portais);
            
            for (int i = 0; i < qnt_f; i++) {
                mover_fantasma(&array_fantasmas[i], array_fantasmas, qnt_f, mapa);
                checar_teleporte(&array_fantasmas[i], portais, qnt_portais);
            }
            verificar_colisao_pacman_fantasma(&pacman, array_fantasmas, qnt_f, power_up_ativo, pos_inicial_pacman );
        
        }
        }
    
    

    // ~~~~ 6) Libera memória ~~~~ //
    CloseWindow();
    free(array_fantasmas);
    free(portais);
    return 0;

}
