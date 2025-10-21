#include <stdio.h>
#include "raylib.h"

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
    InitWindow(800, 400, "pacman");
    SetTargetFPS(10);
    
    while(!WindowShouldClose()){
        int aux1 = x, aux2 = y;
        BeginDrawing();
        ClearBackground(WHITE);
        //movimento
        if(IsKeypressed(KEY_UP)){
            x--;
        }
        if(IsKeypressed(KEY_DOWN)){
            x++;
        }
        if(IsKeypressed(KEY_RIGHT)){
            y++;
        }
        if(IsKeyPressed(KEY_LEFT)){
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
    printf("%d %d", x, y);

    return 0;

}