#include <stdio.h>
#include "raylib.h"

typedef struct{
    int linha;
    int coluna;
}posicao;

int main(){
    FILE *arq;
    arq = fopen("C:\\Users\\dotto\\Downloads\\labirinto.txt", "r");
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
    SetTargetFPS(10);
    int dirx, diry;
    while(!WindowShouldClose()){
        int aux1 = x, aux2 = y;
        BeginDrawing();
        ClearBackground(WHITE);
        //movimento
        if(IsKeyDown(KEY_UP)){
            x--;
            dirx = 0;
            diry = 1;
        }
        if(IsKeyDown(KEY_DOWN)){
            x++;
            dirx = 0;
            diry = 1;
        }
        if(IsKeyDown(KEY_RIGHT)){
            y++;
            dirx = -1;
            diry = 0;
        }
        if(IsKeyDown(KEY_LEFT)){
            y--;
            dirx  = -1;
            diry = 0;
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
        
                if(mapa[i][j] == 'F'){
                    DrawRectangle(j*20, i*20, 20, 20, PURPLE);
                }
                if(mapa[i][j] == 'T'){
                    DrawCircle(j*20 + 10, i*20 + 10, 7, ORANGE);
                }
            }
        }
        DrawRectangle(y*20, x*20, 20, 20, RED);
        EndDrawing();
    }
    CloseWindow();
    //printf("1 - %d %d  || 2 - %d %d ", portais[0].linha, portais[0].coluna, portais[1].linha, portais[1].coluna);
    return 0;

}
