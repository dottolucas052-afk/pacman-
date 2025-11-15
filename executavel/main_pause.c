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
    char controle[20][41];
    char mapa[20][41];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 40; j++) {
            int c = fgetc(arq);
            while (c == '\r' || c == '\n') c = fgetc(arq); // ignora quebra de linha
            if (c == EOF) c = ' ';
            mapa[i][j] = (char)c;
            controle[i][j] = (char)c;
        }
        mapa[i][40] = '\0';
        controle[i][40] = '\0';
    }
    fclose(arq);

    // ~~~~ 2) Inicializações ~~~~ //
    tipo_objeto *array_fantasmas = NULL;
    int qnt_f = 0;
    int pellets = 0;
    int vidas = 3;
    int *v = &vidas;
    int nivel = 1;
    tipo_objeto pacman;
    pacman.velocidade = 7.0f;
    tipo_posicao pos_inicial_pacman;

    // Conta portais (T)
    int contadorT = 0;
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 40; j++)
            if (mapa[i][j] == 'T') contadorT++;

    tipo_posicao *portais = NULL;
    Color AZUL_ESCURO = (Color){ 0, 40, 100, 255 };   
    Color AZUL_NOITE      = (Color){ 5, 25, 70, 255 };    
    Color AZUL_MARINHO    = (Color){ 0, 0, 80, 255 };     
    Color ROXO_ESCURO = (Color){ 40, 0, 100, 255 };
    int qnt_portais = 0;
    if (contadorT > 0) portais = malloc(contadorT * sizeof(tipo_posicao));

    int pontos = 0;
    char texto_pontuacao[30];
    char texto_pellets[30];
    char texto_vida[30];
    char texto_vitoria[30];
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
                array_fantasmas[qnt_f -1].velocidade = 7.0f;
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
            if(mapa[i][j] == '.'|| mapa[i][j] == 'o'){
                pellets++;
            }
        }
    }

    // ~~~~ 4) Inicializa Raylib ~~~~ //
    const int CELULA = 20;
    const int LARGURA = 40 * CELULA;
    const int ALTURA = 20 * CELULA;


    InitWindow(LARGURA, ALTURA + 40, "Pacman");
    srand(time(NULL));
    SetTargetFPS(60);
    double TempoInicio = 0.0;
    double TempoAtual = 0.0;
    // intervalo entre um desenho e outro

    bool jogo_pausado = false;
    bool venceu = false;
    const float intervalo = 1.0f / 4.0f;
    float intervalo_pacman = 1.0f/pacman.velocidade;
    float intervalo_fantasmas = 1.0f/7.0f;
    float contador_tempo = 0.0f;
    float contador_pacman = 0.0f;
    float contador_fantasmas = 0.0f;
    
    Vector2 pacman_pos_visual = { pacman.posicao.coluna * CELULA, pacman.posicao.linha * CELULA };
    const float VELOCIDADE_PACMAN = 100.0f;
    

    // Variáveis do Power-Up
    bool power_up_ativo = false;
    int power_up_timer = 0;
    const int TEMPO_POWER_UP = 40; 
    
   

    //tela inicial
    bool Tela_inicial = true;
    int titulo = MeasureText("PACMAN", 60);
    int novo_jogo = MeasureText("Pressione ENTER para iniciar", 20);
    int carregar_jogo = MeasureText("Pressione C para carregar jogo salvo", 20);
    int opcoes = MeasureText("Pressione O para opções", 20);
    int sair = MeasureText("Pressione ESC para sair", 20);


    while(Tela_inicial && !WindowShouldClose()){
        BeginDrawing();
            ClearBackground(AZUL_MARINHO);
            DrawText("PACMAN", (LARGURA - titulo)/2, 80, 60, YELLOW);
            DrawText("Pressione ENTER para iniciar", (LARGURA - novo_jogo)/2, 200, 20, WHITE);
            DrawText("Pressione C para carregar jogo salvo", (LARGURA - carregar_jogo)/2, 240, 20, WHITE);
            DrawText("Pressione O para opções", (LARGURA - opcoes)/2, 280, 20, WHITE);
            DrawText("Pressione ESC para sair", (LARGURA - sair)/2, 320, 20, WHITE);
        EndDrawing();
        if (IsKeyPressed(KEY_ENTER)) {
        Tela_inicial = false;
    }
    else if (IsKeyPressed(KEY_C)) {
        // Lógica para carregar jogo salvo
        Tela_inicial = false;
    }
    else if (IsKeyPressed(KEY_O)) {
        // Lógica para opções
        // Se der tempo
        Tela_inicial = false;
    }
    
    else if (WindowShouldClose()) {//permite sair durante loading
        CloseWindow();
        free(array_fantasmas);
        free(portais);
        return 0;
    }
    }
    

 // Níveis
    char Texto_inicial[8] = "NÍVEL 1";
    int larguraTexto = MeasureText(Texto_inicial, 40);
    int posX = (GetScreenWidth() - larguraTexto) / 2;
    int posY = (GetScreenHeight() - 40) / 2;

    double loading = 0.0;
    while(loading < 1.0){
        loading += GetFrameTime();
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("NÍVEL 1", posX, posY, 40, RED);
        EndDrawing();
       if (WindowShouldClose()) {//permite sair durante loading
        CloseWindow();
        free(array_fantasmas);
        free(portais);
        return 0;
    }
    }

    if (WindowShouldClose()) {//permite sair durante loading
        CloseWindow();
        free(array_fantasmas);
        free(portais);
        return 0;
    }

    // pellets = 10;
    // ~~~~ 5) Loop principal ~~~~ //
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_TAB)) {
            jogo_pausado = true;
            
        }
        
        if(IsKeyPressed(KEY_V)){
            jogo_pausado = false;
        }

        if(power_up_ativo){
            intervalo_fantasmas = 1.0f/3.0f; 
        } else {
             intervalo_fantasmas = 1.0f/7.0f;
        }
        if(!jogo_pausado){
            contador_pacman += GetFrameTime();
            contador_fantasmas += GetFrameTime();
        }
        float deslize_pacman = contador_pacman / intervalo_pacman;
        float deslize_fantasma  = contador_fantasmas /intervalo_fantasmas;
        
        if(deslize_pacman > 1.0f){
            deslize_pacman = 1.0f;
        }
        if(deslize_fantasma > 1.0f){
            deslize_fantasma = 1.0f;
        }
        snprintf(texto_pontuacao, sizeof(texto_pontuacao), "Pontuação: %d", pontos);
        snprintf(texto_pellets, sizeof(texto_pellets), "Pellets Restantes: %d", pellets);
        snprintf(texto_vida, sizeof(texto_vida), "Vidas: %d", vidas);
        snprintf(texto_vitoria,sizeof(texto_vitoria),  "NÍVEL %d", nivel);
        
        BeginDrawing();
        ClearBackground(AZUL_NOITE);
        
        if(vidas == 0 || IsKeyDown(KEY_X)){
            ClearBackground(AZUL_MARINHO);
            

            int GameOver = MeasureText("Você Perdeu :(", 40);
            DrawText("Você Perdeu :(", LARGURA/2 - GameOver/2, ALTURA/3, 40, YELLOW);
            
            int Again = MeasureText("Pressione R para jogar novamente", 20);
            DrawText("Pressione R para jogar novamente", LARGURA/2 - Again/2+10, ALTURA/2, 20, GOLD);
            DrawRectangle(LARGURA/2 - Again/2, ALTURA/2 - 10 , Again +20, 40, Fade(GOLD, 0.5f));
            
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

    // pellets = 30;
    for (int i = 0, qnt = 0; i < 20; i++) {
        for (int j = 0; j < 41; j++) {
            mapa[i][j] = controle[i][j];
            if (controle[i][j] == '.' || controle[i][j] == 'o') {
                pellets++;
            }
            if(controle[i][j] == 'P'){
                pacman.posicao.linha = i;
                pacman.posicao.coluna = j;

                pos_inicial_pacman.linha = i;
                pos_inicial_pacman.coluna = j;
                pacman.posicao_anterior = pacman.posicao;
                pacman.andar = false;
                pacman.teleportado = false;
            }
            if(controle[i][j] == 'F'){
                qnt++;
                array_fantasmas = realloc(array_fantasmas, qnt_f * sizeof(tipo_objeto));
                array_fantasmas[qnt-1].tipo = FANTASMA;
                array_fantasmas[qnt-1].posicao.linha = i;
                array_fantasmas[qnt-1].posicao.coluna = j;
                array_fantasmas[qnt-1].direcao_atual = CIMA;
                array_fantasmas[qnt-1].proxima_direcao = CIMA;
                array_fantasmas[qnt-1].andar = true;
                array_fantasmas[qnt-1].teleportado = false;
                array_fantasmas[qnt-1].velocidade = 7.0f;
                mapa[i][j] = ' ';
                
            }

        }
        
    }
    power_up_ativo = false;
    DrawRectangle(0, 0, 20, 20, BLACK);
    venceu = false;
    continue;
}


        DrawText(texto_pontuacao, 10, ALTURA - 15, 20, WHITE);
        DrawText(texto_vida, 200, ALTURA - 15, 20, WHITE);
        DrawText(texto_pellets, 350, ALTURA - 15, 20, WHITE);
        int x = pacman.posicao.linha;
        int y = pacman.posicao.coluna;
        
        
        // --- Desenho do Mapa ---
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 40; j++) {
                int px = j * CELULA;
                int py = i * CELULA;
                switch (mapa[i][j]) {
                    case '#': DrawRectangle(px , py , CELULA, CELULA, BLACK); break;
                    case '.': DrawCircle(px + CELULA/2 , py + CELULA/2 , 3, YELLOW); break;
                    case 'o': DrawCircle(px + CELULA/2 , py + CELULA/2 , 6, GREEN); break;
                    case 'T': DrawCircle(px + CELULA/2 , py + CELULA/2 , 5, ORANGE); break;
                }
            }
        }


        Color cor_fantasma = power_up_ativo ? RED : PURPLE; //cor vai depender do estado do fantasma

        for (int i = 0; i < qnt_f; i++) {
            
            float aux_fantasma_x, aux_fantasma_y;
            
            if (array_fantasmas[i].teleportado) {
                aux_fantasma_x = array_fantasmas[i].posicao.coluna * CELULA;
                aux_fantasma_y = array_fantasmas[i].posicao.linha * CELULA;
            } else {
                aux_fantasma_x = (array_fantasmas[i].posicao_anterior.coluna * (1.0f - deslize_fantasma) + array_fantasmas[i].posicao.coluna * deslize_fantasma) * CELULA;
                aux_fantasma_y = (array_fantasmas[i].posicao_anterior.linha * (1.0f - deslize_fantasma) + array_fantasmas[i].posicao.linha * deslize_fantasma) * CELULA;
            }

            DrawRectangle(aux_fantasma_x , aux_fantasma_y , CELULA, CELULA, cor_fantasma);
        }
        
        

        // --- Desenha Pacman ---
        float aux_pacman_x, aux_pacman_y;

        if (pacman.teleportado) {
            aux_pacman_x = pacman.posicao.coluna * CELULA;
            aux_pacman_y = pacman.posicao.linha * CELULA;
        } else {
            aux_pacman_x = (pacman.posicao_anterior.coluna * (1.0f - deslize_pacman) + pacman.posicao.coluna * deslize_pacman) * CELULA;
            aux_pacman_y = (pacman.posicao_anterior.linha * (1.0f - deslize_pacman) + pacman.posicao.linha * deslize_pacman) * CELULA;
        }
        DrawCircle(aux_pacman_x + CELULA / 2 , aux_pacman_y + CELULA / 2 , 9, GOLD);
       
       
        

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
                // Reinicia o jogo
                vidas = 3;
                pontos = 0;
                nivel = 1;
                pellets = 0;

                // 1) Zera fantasmas antigos
                free(array_fantasmas);
                array_fantasmas = NULL;
                qnt_f = 0;

                // 2) Varre o mapa de controle e reseta tudo
                for (int i = 0; i < 20; i++) {
                    for (int j = 0; j < 41; j++) {

                mapa[i][j] = controle[i][j];

                if (controle[i][j] == '.' || controle[i][j] == 'o') {
                    pellets++;
                }

                // Recarrega PACMAN
                if (controle[i][j] == 'P') {
                    pacman.posicao.linha = i;
                    pacman.posicao.coluna = j;

                    pos_inicial_pacman.linha = i;
                    pos_inicial_pacman.coluna = j;

                    pacman.posicao_anterior = pacman.posicao;
                    pacman.andar = false;
                    pacman.teleportado = false;
                }

                // **Recarrega FANTASMAS**
                if (controle[i][j] == 'F') {

                    qnt_f++;
                    array_fantasmas = realloc(array_fantasmas, qnt_f * sizeof(tipo_objeto));

                    array_fantasmas[qnt_f - 1].tipo = FANTASMA;
                    array_fantasmas[qnt_f - 1].posicao.linha = i;
                    array_fantasmas[qnt_f - 1].posicao.coluna = j;

                    array_fantasmas[qnt_f - 1].posicao_anterior = array_fantasmas[qnt_f - 1].posicao;

                    array_fantasmas[qnt_f - 1].direcao_atual = CIMA;
                    array_fantasmas[qnt_f - 1].proxima_direcao = CIMA;

                    array_fantasmas[qnt_f - 1].andar = true;
                    array_fantasmas[qnt_f - 1].teleportado = false;

                    mapa[i][j] = ' ';   
                }

                        }
                    }
                    jogo_pausado = false;
                }

                if(IsKeyPressed(KEY_Q)) {
            
                    
                }
        }

        EndDrawing();
        

       

        if(contador_pacman >= intervalo_pacman){
            contador_pacman -= intervalo_pacman;
            if (mapa[pacman.posicao.linha][pacman.posicao.coluna] != 'T') {
                 pacman.teleportado = false;
            }
            

            pacman.posicao_anterior = pacman.posicao;


            if (!jogo_pausado) {
                
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
                    pontos += 10;
                    if(pellets == 0 && !venceu){
                        nivel++;
                        venceu = true;
                        TempoInicio = GetTime();
                    }
                } else if (mapa[x][y] == 'o') {
                    mapa[x][y] = ' ';
                    pellets--;
                    power_up_ativo = true;
                    power_up_timer = TEMPO_POWER_UP;
                    pontos += 50;
                    if(pellets == 0 && !venceu){
                        nivel++;
                        venceu = true;
                        TempoInicio = GetTime();
                    }
                }

                checar_teleporte(&pacman, portais, qnt_portais);
                verificar_colisao_pacman_fantasma(&pacman, array_fantasmas, qnt_f, power_up_ativo, pos_inicial_pacman, &pontos, v);
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
                        power_up_timer--;
                        if (power_up_timer <= 0) {
                            intervalo_fantasmas = 1.0f/7.0f;
                            power_up_ativo = false;
                        }
                    }
                }
                for (int i = 0; i < qnt_f; i++) {
                    mover_fantasma(&array_fantasmas[i], array_fantasmas, qnt_f, mapa);
                    checar_teleporte(&array_fantasmas[i], portais, qnt_portais);
                }
                //verificar_colisao_pacman_fantasma(&pacman, array_fantasmas, qnt_f, power_up_ativo, pos_inicial_pacman, &pontos );

            }
            
        
    }


    // ~~~~ 6) Libera memória ~~~~ //
    CloseWindow();
    free(array_fantasmas);
    free(portais);
    return 0;

}
