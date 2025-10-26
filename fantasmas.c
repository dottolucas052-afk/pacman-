#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <time.h>


typedef struct{
    int linha;
    int coluna;
}posicao;

int main(){
    FILE *arq;
    arq = fopen("C:/Users/dotto/Downloads/mapa_pacman.txt",  "r");
    if(arq == NULL){
        printf("ERRO NA ABERTURA DO ARQUIVO \n");
        return 0;
    }
    
    char mapa[20][41] = {'\0'};
    for(int i = 0; i<20; i++){
        fgets(mapa[i], 41, arq);
    }

    fclose(arq);
    int x = 0 , y = 0;
    for(int i = 0; i<20; i++){
        for(int j = 0; j<40; j++){
            if(mapa[i][j] == 'P'){
                x = i;
                y = j;
            }
        }
    }
    int qnt_f = 0;
    for(int i = 0; i<20; i++){
        for(int j = 0; j<40; j++){
            if(mapa[i][j] == 'F'){
                qnt_f++;
                //mapa[i][j] = ' ';
            }
        }
    }
    posicao *fantasmas = malloc(sizeof(posicao) * qnt_f);
    int indice_fantasma = 0; 
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 40; j++){
            if(mapa[i][j] == 'F'){
                fantasmas[indice_fantasma].linha = i;
                fantasmas[indice_fantasma].coluna = j;
                mapa[i][j] = ' ';
                indice_fantasma++; 
            }
        }   
    }

    posicao portais[10];
    int qnt_portais = 0;
    for(int i = 0; i<20; i++){
        for(int j = 0; j<40; j++){
            if(mapa[i][j] == 'T'){
                portais[qnt_portais].linha = i;
                portais[qnt_portais].coluna = j; 
                qnt_portais++;   
            }
        }
    }

    InitWindow(800, 400, "pacman");
    srand(time(NULL));
    SetTargetFPS(10);
    int dirx, diry;
    while(!WindowShouldClose()){
        int aux1 = x, aux2 = y;
        BeginDrawing();
        ClearBackground(WHITE);
        //movimento
        if(IsKeyDown(KEY_UP)){
            x--;
        }
        if(IsKeyDown(KEY_DOWN)){
            x++;
        }
        if(IsKeyDown(KEY_RIGHT)){
            y++;
        }
        if(IsKeyDown(KEY_LEFT)){
            y--;
        }
        
        //controle de colisão
        if(mapa[x][y] == '#'){
            x = aux1;
            y = aux2;
        }
        if(mapa[x][y] == '.'){
            mapa[x][y] = ' ';
        }
        if(mapa[x][y] == 'o'){
            mapa[x][y] = ' ';
        }
        for(int i = 0; i< qnt_f; i++){
            int aux1 = fantasmas[i].linha, aux2 = fantasmas[i].coluna;
            int dx = 0, dy = 0;
            int aleatorio = rand()%2;
            if(aleatorio == 0){
                if(rand() % 100 > 70){
                if(x > fantasmas[i].linha){
                    fantasmas[i].linha++;
                }else if(x < fantasmas[i].linha){
                    fantasmas[i].linha--;
                }
                if(y > fantasmas[i].coluna){
                    fantasmas[i].coluna++;
                }else if(y < fantasmas[i].coluna){
                    fantasmas[i].coluna--;
                }
            }else{
                int dir = rand() % 4;
                if(dir == 0){
                   fantasmas[i].linha++; 
                }
                if(dir == 1){
                    fantasmas[i].linha--;
                }
                if(dir == 2){
                    fantasmas[i].coluna++;
                }
                if(dir == 3){
                    fantasmas[i].coluna--;
                }

            } 
        }   
            if(mapa[fantasmas[i].linha][fantasmas[i].coluna] == '#'){
                fantasmas[i].coluna = aux2;
                fantasmas[i].linha = aux1;
            }           
        
            if(mapa[fantasmas[i].linha][fantasmas[i].coluna] == 'T'){
                for(int i = 0; i< qnt_portais; i++){
                    if(!(fantasmas[i].linha == portais[i].linha && fantasmas[i].coluna == portais[i].coluna)){
                        if(fantasmas[i].linha == portais[i].linha ||fantasmas[i].coluna == portais[i].coluna){
                            fantasmas[i].linha = portais[i].linha ;
                            fantasmas[i].coluna = portais[i].coluna ;
                            if(fantasmas[i].coluna == 0){
                                fantasmas[i].coluna++;
                            }else{
                                fantasmas[i].coluna--;
                            }
                            break;
                        }
            
                    }
                }                
            }
            
        }
        //Teleporte
        if(mapa[x][y] == 'T'){
            for(int i = 0; i< qnt_portais; i++){
                if(!(x == portais[i].linha && y == portais[i].coluna)){
                    if(x == portais[i].linha || y == portais[i].coluna){
                        x = portais[i].linha ;
                        y = portais[i].coluna ;
                        if(y == 0){
                            y++;
                        }else{
                            y--;
                        }
                        break;
                    }
         
                }
            }
        }

        for(int i = 0; i<20; i++){
            
            for(int j = 0; j<40; j++){

                if(mapa[i][j] == '#'){
                    DrawRectangle(j*20, i*20, 20, 20, BLACK);
                }
                if(mapa[i][j] == '.'){
                    DrawCircle(j*20 + 10, i*20 + 10, 5, YELLOW);
                }
                if(mapa[i][j] == 'o'){
                    DrawCircle(j*20 + 10, i*20 + 10, 10, GREEN);
                }
        
                // if(mapa[i][j] == 'F'){
                //     DrawRectangle(j*20, i*20, 20, 20, PURPLE);
                // }
                if(mapa[i][j] == 'T'){
                    DrawCircle(j*20 + 10, i*20 + 10, 7, ORANGE);
                }
                
            }
        }
        DrawRectangle(y*20, x*20, 20, 20, RED);
        for(int i = 0; i<qnt_f; i++){
            DrawRectangle(fantasmas[i].coluna*20, fantasmas[i].linha*20, 20, 20, PURPLE);
        }
        EndDrawing();
    }
    CloseWindow();
    //printf("1 - %d %d  || 2 - %d %d ", portais[0].linha, portais[0].coluna, portais[1].linha, portais[1].coluna);
    return 0;

}

