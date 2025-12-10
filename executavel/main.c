#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "raylib.h"
#include "funcoes.h"

int main() {
    int nivel = 1;
    FILE *arq = NULL;
    tipo_objeto *array_fantasmas = NULL;
    int qnt_f = 0;
    int pellets = 0;
    int vidas = 3;
    int *v = &vidas;
    tipo_objeto pacman;
    pacman.velocidade = 7.0f;
    tipo_posicao pos_inicial_pacman;
    float pacman_rotacao = 0.0f;
    int qnt_portais = 0;
    tipo_posicao *portais = NULL;

    char controle[20][41];
    char mapa[20][41];

    // Carrega o primeiro mapa
    inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
    
    Color AZUL_ESCURO = (Color){ 0, 40, 100, 255 };   
    Color AZUL_NOITE      = (Color){ 5, 25, 70, 255 };    
    Color AZUL_MARINHO    = (Color){ 0, 0, 80, 255 };     
    Color ROXO_ESCURO = (Color){ 40, 0, 100, 255 };

    int pontos = 0;
    char texto_pontuacao[30];
    char texto_pellets[30];
    char texto_vida[30];
    char texto_vitoria[30];

    bool pacman_morrendo = false;
    float timer_morte_pacman = 0.0f;
    const float TEMPO_MORTE = 2.0f; 

    const int CELULA = 20; 
    const int LARGURA = 40 * CELULA;
    const int ALTURA = 20 * CELULA;

    InitWindow(LARGURA, ALTURA + 40, "Pacman");
    srand(time(NULL));
    SetTargetFPS(60);

    Texture2D textura_fantasma_normal = LoadTexture("sprites/sprite_fantasma.png");
    Texture2D textura_fantasma_nervoso = LoadTexture("sprites/fantasma_nervoso.png");
    Texture2D textura_pacman = LoadTexture("sprites/pacman.gif");
    Texture2D textura_teleporte = LoadTexture("sprites/teleporte.png");
    Texture2D textura_logo = LoadTexture("sprites/logo.png");

    double TempoInicio = 0.0;
    double TempoAtual = 0.0;

    bool jogo_pausado = false;
    bool venceu = false;
    
    float intervalo_pacman = 1.0f/pacman.velocidade;
    float intervalo_fantasmas = 1.0f/7.0f;
    float contador_pacman = 0.0f;
    float contador_fantasmas = 0.0f;
    
    bool power_up_ativo = false;
    int power_up_timer = 0;
    const int TEMPO_POWER_UP = 40; 
    int verificador_cor = 40;
    
    //tela inicial
    bool Tela_inicial = true;
    int novo = MeasureText("Pressione ENTER para iniciar", 20);
    int carregar = MeasureText("Pressione C para carregar jogo salvo", 20);
    int opcoes = MeasureText("Pressione O para opções", 20);
    int sair = MeasureText("Pressione ESC para sair", 20);

    const int LOGO_LARGURA_DESEJADA = 300;
    const int LOGO_ALTURA_DESEJADA = (int)(((float)textura_logo.height / textura_logo.width) * LOGO_LARGURA_DESEJADA); 
    int logo_x = (LARGURA - LOGO_LARGURA_DESEJADA) / 2;
    int logo_y = 20;

    bool jogo_rodando = true;
    while (jogo_rodando && !WindowShouldClose()) {
        while(Tela_inicial && !WindowShouldClose()){
            BeginDrawing();
                ClearBackground(AZUL_MARINHO);

                DrawTexturePro(textura_logo,(Rectangle){ 0.0f, 0.0f, (float)textura_logo.width, (float)textura_logo.height },(Rectangle){ (float)logo_x, (float)logo_y, (float)LOGO_LARGURA_DESEJADA, (float)LOGO_ALTURA_DESEJADA },(Vector2){ 0, 0 }, 0.0f, WHITE);
              
                DrawRectangle((LARGURA - carregar)/2-20, 200 - 50, carregar +40, 40, Fade(GOLD, 0.5f));
                DrawText("Pressione ENTER para iniciar", (LARGURA - novo)/2, 200-40, 20, WHITE);
               
                DrawRectangle((LARGURA - carregar)/2-20, 240 - 30, carregar +40, 40, Fade(GOLD, 0.5f));
                DrawText("Pressione C para carregar jogo salvo", (LARGURA - carregar)/2, 240-20, 20, WHITE);

                DrawRectangle((LARGURA - carregar)/2-20, 280 - 10, carregar +40, 40, Fade(GOLD, 0.5f));
                DrawText("Pressione O para opções", (LARGURA - opcoes)/2, 280, 20, WHITE);

                DrawRectangle((LARGURA - carregar)/2-20, 320 + 10, carregar +40, 40, Fade(GOLD, 0.5f));
                DrawText("Pressione ESC para sair", (LARGURA - sair)/2, 320+20, 20, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                Tela_inicial = false;
            }
            else if (IsKeyPressed(KEY_C)) {
                Tela_inicial = false;
                if (carregar_jogo(mapa, &vidas, &pontos, &nivel, &pellets, &pacman, &array_fantasmas, &qnt_f, &power_up_ativo, &power_up_timer, "saves/save1.txt")) {
                    jogo_pausado = false; 
                    contador_fantasmas = 0.0f; 
                    contador_pacman = 0.0f;
                    for(int x = 0; x < 20; x++){
                        for(int y = 0; y < 41; y++){
                            controle[x][y] = mapa[x][y];
                        }
                    }
                } 
                else {
                    printf("Falha ao carregar o jogo do arquivo: %s\n", "saves/save1.txt");
                }
            }
            else if (IsKeyPressed(KEY_O)) {
                Tela_inicial = false;
            }
            else if (WindowShouldClose()) {
                CloseWindow();
                if(array_fantasmas) free(array_fantasmas);
                if(portais) free(portais);
                return 0;
            }
        }

        char Texto_inicial[20];
        snprintf(Texto_inicial, sizeof(Texto_inicial), "NÍVEL %d", nivel);
        int larguraTexto = MeasureText(Texto_inicial, 40);
        int posX = (GetScreenWidth() - larguraTexto) / 2;
        int posY = (GetScreenHeight() - 40) / 2;

        double loading = 0.0;
        if (nivel == 1 && contador_pacman == 0.0f && pontos == 0 && !pacman_morrendo) {
            while(loading < 1.0){
                loading += GetFrameTime();
                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawText("NÍVEL 1", posX, posY, 40, RED);
                EndDrawing();
               if (WindowShouldClose()) {
                    CloseWindow();
                    if(array_fantasmas) free(array_fantasmas);
                    if(portais) free(portais);
                    return 0;
                }
            }
        }

        // ~~~~ 5) Loop principal ~~~~ //
        while (!WindowShouldClose()) {

            if (IsKeyPressed(KEY_TAB)) {
                jogo_pausado = true;
            }
            if(IsKeyPressed(KEY_V)){
                jogo_pausado = false;
            }

            // --- LÓGICA DE MORTE COM DELAY ---
            if (pacman_morrendo) {
                timer_morte_pacman -= GetFrameTime();
                if (timer_morte_pacman <= 0.0f) {
                    vidas--;
                    pacman.posicao = pos_inicial_pacman;
                    pacman.posicao_anterior = pos_inicial_pacman;
                    pacman.andar = false;
                    pacman.direcao_atual = DIREITA; 
                    pacman_rotacao = 0.0f;
                    pacman_morrendo = false; 
                }
            }

            if(power_up_ativo){
                intervalo_fantasmas = 1.0f/5.0f; 
            } else {
                 intervalo_fantasmas = 1.0f/7.0f;
            }
            
            if(!jogo_pausado && !pacman_morrendo){
                contador_pacman += GetFrameTime();
                contador_fantasmas += GetFrameTime();
            }

            // --- Lógica de Renascimento dos Fantasmas (Level 2+) ---
            if (nivel >= 2 && !jogo_pausado) {
                for (int i = 0; i < qnt_f; i++) {
                    if (!array_fantasmas[i].ativo && array_fantasmas[i].timer_respawn > 0) {
                        array_fantasmas[i].timer_respawn -= GetFrameTime();
                        
                        if (array_fantasmas[i].timer_respawn <= 0) {
                            tipo_posicao spawns[50];
                            int qtd_spawns = 0;
                            for (int r = 0; r < 20; r++) {
                                for (int c = 0; c < 41; c++) {
                                    if (controle[r][c] == 'F') {
                                        spawns[qtd_spawns].linha = r;
                                        spawns[qtd_spawns].coluna = c;
                                        qtd_spawns++;
                                    }
                                }
                            }
                            int spawn_escolhido = 0;
                            for (int k = 0; k < qtd_spawns; k++) {
                                int dist = abs(pacman.posicao.linha - spawns[k].linha) + 
                                           abs(pacman.posicao.coluna - spawns[k].coluna);
                                if (dist > 5) { 
                                    spawn_escolhido = k;
                                    break;
                                }
                            }
                            if (qtd_spawns > 0) {
                                array_fantasmas[i].posicao = spawns[spawn_escolhido];
                                array_fantasmas[i].posicao_anterior = spawns[spawn_escolhido];
                                array_fantasmas[i].ativo = true;
                            }
                        }
                    }
                }
            }

            float deslize_pacman = contador_pacman / intervalo_pacman;
            float deslize_fantasma  = contador_fantasmas /intervalo_fantasmas;
            
            if(deslize_pacman > 1.0f) deslize_pacman = 1.0f;
            if(deslize_fantasma > 1.0f) deslize_fantasma = 1.0f;

            snprintf(texto_pontuacao, sizeof(texto_pontuacao), "Pontuação: %d", pontos);
            snprintf(texto_pellets, sizeof(texto_pellets), "Pellets Restantes: %d", pellets);
            snprintf(texto_vida, sizeof(texto_vida), "Vidas: %d", vidas);
            snprintf(texto_vitoria,sizeof(texto_vitoria),  "NÍVEL %d", nivel);
            
            BeginDrawing();
            ClearBackground(AZUL_NOITE);
            
            if(vidas == 0){
                ClearBackground(AZUL_MARINHO);
                int GameOver = MeasureText("Você Perdeu :(", 40);
                DrawText("Você Perdeu :(", LARGURA/2 - GameOver/2, ALTURA/3, 40, YELLOW);
                int Again = MeasureText("Pressione R para jogar novamente", 20);
                DrawText("Pressione R para jogar novamente", LARGURA/2 - Again/2+10, ALTURA/2, 20, GOLD);
                DrawRectangle(LARGURA/2 - Again/2, ALTURA/2 - 10 , Again +20, 40, Fade(GOLD, 0.5f));
            
                if (IsKeyPressed(KEY_R)) {
                    contador_fantasmas = 0;
                    contador_pacman = 0;
                    vidas = 3;
                    pontos = 0;
                    nivel = 1;
                    pellets = 0;
                    qnt_f = 0;
                    qnt_portais = 0;
                    power_up_ativo = false;
                            
                    inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
                    
                    jogo_pausado = false;
                }
                    
                TempoAtual += GetFrameTime();
                if(TempoAtual > 8000.0){
                    EndDrawing();
                    break;
                }
                EndDrawing();
                continue;
            }

            if (venceu) {
                float TempoAtual = 0.0f;
                while (TempoAtual < 5.0f) {
                    BeginDrawing();
                    ClearBackground(BLACK);
                    DrawText(texto_vitoria, posX, posY, 40, RED);
                    EndDrawing();
                    TempoAtual += GetFrameTime();
                }
                venceu = false;
                power_up_ativo = false;
                power_up_timer = 0;
                continue;
            }

            DrawText(texto_pontuacao, 10, ALTURA+10, 20, YELLOW);
            DrawText(texto_vida, 215, ALTURA+10, 20, YELLOW);
            DrawText(texto_pellets, 350, ALTURA+10, 20, YELLOW);
            
            for (int i = 0; i < 20; i++) {
                for (int j = 0; j < 40; j++) {
                    int px = j * CELULA;
                    int py = i * CELULA;
                    switch (mapa[i][j]) {
                        case '#': DrawRectangle(px , py , CELULA, CELULA, BLACK); break;
                        case '.': DrawCircle(px + CELULA/2 , py + CELULA/2 , 3, YELLOW); break;
                        case 'o': DrawCircle(px + CELULA/2 , py + CELULA/2 , 6, GREEN); break;
                        case 'T': DrawTexturePro(textura_teleporte, (Rectangle){ 0.0f, 0.0f, (float)textura_teleporte.width, (float)textura_teleporte.height }, (Rectangle){ (float)px, (float)py, (float)CELULA, (float)CELULA }, (Vector2){ 0, 0 }, 0.0f, WHITE); break;
                    }
                }
            }

            for (int i = 0; i < qnt_f; i++) {
                if (!array_fantasmas[i].ativo) continue;

                float aux_fantasma_x, aux_fantasma_y;
                if (array_fantasmas[i].teleportado) {
                    aux_fantasma_x = array_fantasmas[i].posicao.coluna * CELULA;
                    aux_fantasma_y = array_fantasmas[i].posicao.linha * CELULA;
                } else {
                    aux_fantasma_x = (array_fantasmas[i].posicao_anterior.coluna * (1.0f - deslize_fantasma) + array_fantasmas[i].posicao.coluna * deslize_fantasma) * CELULA;
                    aux_fantasma_y = (array_fantasmas[i].posicao_anterior.linha * (1.0f - deslize_fantasma) + array_fantasmas[i].posicao.linha * deslize_fantasma) * CELULA;
                }
                Rectangle destino_f = { aux_fantasma_x, aux_fantasma_y, (float)CELULA, (float)CELULA };
                Rectangle origem_f;
                Vector2 origem_desenho_f = { 0.0f, 0.0f };

                if(power_up_timer >= 15){
                    origem_f = (Rectangle){ 0.0f, 0.0f, (float)textura_fantasma_nervoso.width, (float)textura_fantasma_nervoso.height };
                    DrawTexturePro(textura_fantasma_nervoso, origem_f, destino_f, origem_desenho_f, 0.0f, WHITE);
                }
                else if (power_up_timer< 15 && power_up_timer % 2 == 1){
                    origem_f = (Rectangle){ 0.0f, 0.0f, (float)textura_fantasma_nervoso.width, (float)textura_fantasma_nervoso.height };
                    DrawTexturePro(textura_fantasma_nervoso, origem_f, destino_f, origem_desenho_f, 0.0f, WHITE);
                }
                else if (power_up_timer < 15 && power_up_timer % 2 == 0){
                    origem_f = (Rectangle){ 0.0f, 0.0f, (float)textura_fantasma_normal.width, (float)textura_fantasma_normal.height };
                    DrawTexturePro(textura_fantasma_normal, origem_f, destino_f, origem_desenho_f, 0.0f, WHITE);
                }
            }
            
            float aux_pacman_x, aux_pacman_y;
            if (pacman.teleportado) {
                aux_pacman_x = pacman.posicao.coluna * CELULA;
                aux_pacman_y = pacman.posicao.linha * CELULA;
            } else {
                aux_pacman_x = (pacman.posicao_anterior.coluna * (1.0f - deslize_pacman) + pacman.posicao.coluna * deslize_pacman) * CELULA;
                aux_pacman_y = (pacman.posicao_anterior.linha * (1.0f - deslize_pacman) + pacman.posicao.linha * deslize_pacman) * CELULA;
            }
            Rectangle origem_p = { 0.0f, 0.0f, (float)textura_pacman.width, (float)textura_pacman.height };
            Vector2 origem_desenho_p = { (float)CELULA / 2.0f, (float)CELULA / 2.0f };
            Rectangle destino_p = { aux_pacman_x + origem_desenho_p.x, aux_pacman_y + origem_desenho_p.y, (float)CELULA, (float)CELULA};
            
            Color cor_pacman = WHITE;
            if (pacman_morrendo) {
                if (((int)(timer_morte_pacman * 10)) % 2 == 0) cor_pacman = RED; 
            }
            DrawTexturePro(textura_pacman, origem_p, destino_p, origem_desenho_p, pacman_rotacao, cor_pacman);

            if (jogo_pausado) {
                int pause = MeasureText("Pressione TAB para pausar", 20);
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.50f));
                DrawText("JOGO PAUSADO",LARGURA/2-pause/2,60, 40, WHITE);
                int new = MeasureText("Novo Jogo(N)", 20);
                DrawRectangle(LARGURA/3-new/2, ALTURA/2, new+20, 40, LIGHTGRAY);
                DrawText("Novo Jogo(N)", LARGURA/3-new/2+10, ALTURA/2 +10, 20, BLACK);
                int quit = MeasureText("Sair(Q)", 20);
                DrawRectangle(LARGURA/3-new/2+new+20+10, ALTURA/2 , quit+20, 40, LIGHTGRAY);
                DrawText("Sair(Q)",LARGURA/3-new/2+new+20+10+10 , ALTURA/2 +10, 20, BLACK);
                int save = MeasureText("Salvar Jogo(S)", 20);
                DrawRectangle(LARGURA/3-new/2+new+20+10+quit+20+10, ALTURA/2 , save+20, 40, LIGHTGRAY);
                DrawText("Salvar Jogo(S)",LARGURA/3-new/2+new+20+10+quit+20+10+10 , ALTURA/2 +10, 20, BLACK);
                int load = MeasureText("Carregar Jogo(C)", 20);
                DrawRectangle(LARGURA/2-load-10, ALTURA/2+50 , load+20, 40, LIGHTGRAY);
                DrawText("Carregar Jogo(C)",LARGURA/2-load-10+10 , ALTURA/2 +50+ 10, 20, BLACK);
                int back = MeasureText("Voltar ao Jogo(V)", 20);
                DrawRectangle(LARGURA/2-load+load+10+10, ALTURA/2+50 , back+20, 40, LIGHTGRAY);
                DrawText("Voltar ao Jogo(V)",LARGURA/2-load+load+10+10+10 , ALTURA/2 +50+ 10, 20, BLACK);
                
                if (IsKeyPressed(KEY_N)) {
                    vidas = 3;
                    pontos = 0;
                    nivel = 1;
                    pellets = 0;
                    inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
                    jogo_pausado = false;
                    power_up_ativo = false;
                    power_up_timer = 0;
                }
                if(IsKeyPressed(KEY_S)) {
                    salvar_jogo(mapa, vidas, pontos, nivel, pellets, pacman, array_fantasmas, qnt_f, power_up_ativo, power_up_timer);
                }
                if (IsKeyPressed(KEY_C)) {
                    char nome_arquivo_para_carregar[64];
                    strcpy(nome_arquivo_para_carregar, "saves/save1.txt"); 
                    if (carregar_jogo(mapa, &vidas, &pontos, &nivel, &pellets, &pacman, &array_fantasmas, &qnt_f, &power_up_ativo, &power_up_timer, "saves/save1.txt")) {
                        jogo_pausado = false; 
                        contador_fantasmas = 0.0f; 
                        contador_pacman = 0.0f;
                         for(int x = 0; x < 20; x++){
                            for(int y = 0; y < 41; y++){
                                controle[x][y] = mapa[x][y];
                            }
                        }
                    } else {
                        printf("Falha ao carregar o jogo do arquivo: %s\n", nome_arquivo_para_carregar);
                    }
                }
                if (IsKeyPressed(KEY_Q)) {
                    vidas = 3;
                    pontos = 0;
                    nivel = 1;
                    pellets = 0;
                    inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
                    Tela_inicial = true;
                    jogo_pausado = false;
                    break;
                }
            }
            EndDrawing();

            if(contador_pacman >= intervalo_pacman){
                contador_pacman -= intervalo_pacman;
                if (mapa[pacman.posicao.linha][pacman.posicao.coluna] != 'T') {
                     pacman.teleportado = false;
                }
                pacman.posicao_anterior = pacman.posicao;

                if (!jogo_pausado && !pacman_morrendo) {
                    
                    pacman.andar = false;
                    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {pacman.direcao_atual = CIMA; pacman.andar = true; pacman_rotacao = 270.0f;} 
                    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {pacman.direcao_atual = BAIXO; pacman.andar = true; pacman_rotacao = 90.0f;} 
                    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {pacman.direcao_atual = DIREITA; pacman.andar = true; pacman_rotacao = 0.0f;} 
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {pacman.direcao_atual = ESQUERDA; pacman.andar = true; pacman_rotacao = 180.0f;}
                    
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
                        pontos += 10;
                    } else if (mapa[x][y] == 'o') {
                        mapa[x][y] = ' ';
                        pellets--;
                        power_up_ativo = true;
                        power_up_timer = TEMPO_POWER_UP;
                        pontos += 50;
                    }

                    // --- CHECAGEM DE VITÓRIA / PRÓXIMO NÍVEL ---
                    // Agora está fora dos blocos de colisão individuais!
                    if((pellets == 0 && !venceu) || IsKeyPressed(KEY_K)){
                        nivel++;
                        char nome_proximo_mapa[20];
                        snprintf(nome_proximo_mapa, sizeof(nome_proximo_mapa), "mapa%d.txt", nivel);
                        FILE *teste_mapa = fopen(nome_proximo_mapa, "r");
                        
                        if (teste_mapa == NULL) {
                            double tempo_inicio_tela = GetTime();
                            while (GetTime() - tempo_inicio_tela < 5.0) {
                                BeginDrawing();
                                    ClearBackground(BLACK);
                                    DrawText("PARABÉNS! JOGO FINALIZADO!", LARGURA/2 - 200, ALTURA/2, 30, GOLD);
                                EndDrawing();
                                if (WindowShouldClose()) { CloseWindow(); return 0; }
                            }
                            nivel = 1;
                            vidas = 3;
                            pontos = 0;
                            inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
                            venceu = false;
                            Tela_inicial = true; 
                            jogo_pausado = false;  
                            break; 
                        } else {
                            fclose(teste_mapa);
                            inicializar_mapa(arq, &nivel, &array_fantasmas, mapa, controle, &qnt_portais, &qnt_f, &pellets, &pacman, &pos_inicial_pacman, &portais);
                            venceu = true;
                            TempoInicio = GetTime();
                        }
                    }

                    checar_teleporte(&pacman, portais, qnt_portais);
                    if (!pacman_morrendo) {
                         verificar_colisao_pacman_fantasma(&pacman, array_fantasmas, qnt_f, power_up_ativo, pos_inicial_pacman, &pontos, v, controle, nivel, &pacman_morrendo);
                         if (pacman_morrendo) timer_morte_pacman = TEMPO_MORTE;
                    }
                }
            }

            if(contador_fantasmas >= intervalo_fantasmas){
                    contador_fantasmas -= intervalo_fantasmas;
                    for (int i = 0; i < qnt_f; i++) {
                        if (mapa[array_fantasmas[i].posicao.linha][array_fantasmas[i].posicao.coluna] != 'T') {
                            array_fantasmas[i].teleportado = false;
                        }
                    }
                    for(int i = 0; i< qnt_f; i++){
                        array_fantasmas[i].posicao_anterior = array_fantasmas[i].posicao;
                    }
                    if(!jogo_pausado){
                        if (power_up_ativo) {
                            verificador_cor--;
                            power_up_timer--;
                            if (power_up_timer <= 0) {
                                intervalo_fantasmas = 1.0f/7.0f;
                                power_up_ativo = false;
                            }
                        }
                    }
                    for (int i = 0; i < qnt_f; i++) {
                        if (!array_fantasmas[i].ativo) continue;
                        if (!pacman_morrendo) {
                            mover_fantasma(&array_fantasmas[i], array_fantasmas, qnt_f, mapa);
                            checar_teleporte(&array_fantasmas[i], portais, qnt_portais);
                        }
                    }
            }
        }
    }

    CloseWindow();
    UnloadTexture(textura_teleporte);
    if(array_fantasmas) free(array_fantasmas);
    if(portais) free(portais);
    return 0;
}


