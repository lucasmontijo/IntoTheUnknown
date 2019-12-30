#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
const int WIDTH = 640;
const int HEIGHT = 480;

int main(int argc, char *argv[]) {
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Erro no SDL! SDL_Error: %s\n", SDL_GetError());
	}
	else {
		//Criar janela
		window = SDL_CreateWindow("Into The Unknown", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("A janela não pode ser criada com o erro %s.\n", SDL_GetError());
		}
	}
}