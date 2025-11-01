#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "raylib.h"
#include "funcoes.h"


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

    tipo_objeto pacman;

    tipo_posicao pos_inicial_pacman;

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

                pacman.andar = false;
                pacman.teleportado = false;                           
            }
        }
    }

    // ~~~~ 4) Inicializa Raylib ~~~~ //
    const int CELULA = 20;
    const int LARGURA = 40 * CELULA;
    const int ALTURA = 20 * CELULA;
    InitWindow(LARGURA, ALTURA, "Pacman");
    srand(time(NULL));
    SetTargetFPS(10);
    
    Vector2 pacman_pos_visual = { pacman.posicao.coluna * CELULA, pacman.posicao.linha * CELULA };
    const float VELOCIDADE_PACMAN = 100.0f;

    // Variáveis do Power-Up
    bool power_up_ativo = false;
    int power_up_timer = 0;
    const int TEMPO_POWER_UP = 80; // considerei 8 segundos (ver esquema de velocidade para poder aumentar fps)



    // ~~~~ 5) Loop principal ~~~~ //
    while (!WindowShouldClose()) {


        // Atualiza timer do Power-Up
        if (power_up_ativo) {
            power_up_timer--;
            if (power_up_timer <= 0) {
                power_up_ativo = false;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);
        
        // --- Controle Pacman ---   
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

        //if (mapa[x][y] == '.' || mapa[x][y] == 'o') mapa[x][y] = ' ';antiga lógica de coleta de pellets
        //mudei a logica de coleta de pellets
        if (mapa[x][y] == '.') {
            mapa[x][y] = ' '; 
            //inserir esquema de pontuação
        } else if (mapa[x][y] == 'o') {
            mapa[x][y] = ' ';
            power_up_ativo = true;
            power_up_timer = TEMPO_POWER_UP;
        }


        // --- Portais ---
        checar_teleporte(&pacman, portais, qnt_portais);
        

        // --- Movimento Fantasmas ---
        for (int i = 0; i < qnt_f; i++) {
            mover_fantasma(&array_fantasmas[i], array_fantasmas, qnt_f, mapa);
            checar_teleporte(&array_fantasmas[i], portais, qnt_portais);
        }

        verificar_colisao_pacman_fantasma(&pacman, array_fantasmas, qnt_f, power_up_ativo, pos_inicial_pacman );

        // --- Desenho do Mapa ---
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 40; j++) {
                int px = j * CELULA;
                int py = i * CELULA;
                switch (mapa[i][j]) {
                    case '#': DrawRectangle(px, py, CELULA, CELULA, BLACK); break;
                    case '.': DrawCircle(px + CELULA/2, py + CELULA/2, 3, YELLOW); break;
                    case 'o': DrawCircle(px + CELULA/2, py + CELULA/2, 6, GREEN); break;
                    case 'T': DrawCircle(px + CELULA/2, py + CELULA/2, 5, ORANGE); break;
                }
            }
        }


        Color cor_fantasma = power_up_ativo ? BLUE : PURPLE; //cor vai depender do estado do fantasma
        // --- Desenha Fantasmas ---
        for (int i = 0; i < qnt_f; i++) {
            DrawRectangle(
                array_fantasmas[i].posicao.coluna * CELULA,
                array_fantasmas[i].posicao.linha * CELULA,
                CELULA, CELULA, cor_fantasma
            );
        }

        // --- Desenha Pacman ---
        DrawCircle(y * CELULA + CELULA / 2, x * CELULA + CELULA / 2, 9, GOLD);

        EndDrawing();
    }

    // ~~~~ 6) Libera memória ~~~~ //
    CloseWindow();
    free(array_fantasmas);
    free(portais);
    return 0;
}
