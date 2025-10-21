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
    SetTargetFPS(60);
    
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        if(IsKeyPressed(KEY_UP)){
            x--;
        }
        if(IsKeyPressed(KEY_DOWN)){
            x++;
        }
        if(IsKeyPressed(KEY_RIGHT)){
            y++;
        }
        if(IsKeyPressed(KEY_LEFT)){
            y--;
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
                // if(mapa[i][j] == 'P'){
                //     DrawRectangle(j*20, i*20, 20, 20, RED);

                // }
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