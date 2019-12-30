#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
const int WIDTH = 1280;
const int HEIGHT = 720;
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;

//inicia a janela
bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

//fecha a janela
void close() {
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

bool loadMedia()
{
    bool success = true;

    //carregando a imagem 
    gHelloWorld = SDL_LoadBMP("maquinaDoTempo.bmp");
    if (gHelloWorld == NULL)
    {
        printf("Nao foi possivel carregar a imagem. Erro %s. \n", SDL_GetError());
        success = false;
    }

    return success;
}

int main(int argc, char *argv[]) {
    bool quit = false;
    SDL_Event e;
    init();
    while(!quit){
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
        }
        loadMedia();
        SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
        SDL_UpdateWindowSurface(gWindow);
    }
    close();
	return 0;
}