Este documento explica como compilar e executar o código fonte main.c utilizando a Raylib, tanto em Windows quanto em Linux.
Certifique-se de que a Raylib está instalada corretamente e que as bibliotecas foram configuradas no PATH ou estão na mesma pasta do projeto.

Compilação e execução no windows:

gcc main.c -o main -lraylib -lwinmm -lgdi32 -lopengl32
./teste_raylib.exe

Compilação e execução no linux:

gcc main.c -o main -lraylib -lm -lpthread -ldl -lrt -lX11
./teste_raylib

Obs: Para a execução desse código os mapas devem ser nomeados da seguinte forma: "mapa[i]" (ex: "mapa1", "mapa2", "mapa[3]"). Isso se deve à lógica usada por nós para trocar os mapas do jogo ao longo da execução do código.

