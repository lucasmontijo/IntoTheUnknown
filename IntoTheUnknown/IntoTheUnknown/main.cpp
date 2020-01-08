#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
#include"SDL_image.h"
#include<iostream>
#define WIDTH 960
#define HEIGHT 540

//VARIAVEIS GLOBAIS
int menuStatus;

//RENDERERS


//SURFACES
SDL_Surface* screenSurface;
SDL_Surface* menuSurface;

//TEXTURES 


//WINDOWS
SDL_Window* window;

//EVENTS
SDL_Event eventHappened;

//FUNCTIONS
void startSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL not initialized. Error: %s", SDL_GetError());
    }
    IMG_Init(IMG_INIT_PNG);
}

void createWindow() {
    window = SDL_CreateWindow("Into The Unknown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
}

void startImageFlags() {
    int imgFlags = IMG_INIT_PNG;
    if (!IMG_Init(imgFlags) & imgFlags) {
        printf("SDL image could not initialize with error: %s", IMG_GetError());
    }
    else {
        screenSurface = SDL_GetWindowSurface(window);
        if (!screenSurface) {
            printf("Erro ao pegar srface da janela. SDL erro %s", SDL_GetError());
        }
        else {
            printf("Surface da janela\n");
        }
    }
}

SDL_Surface *loadSurface(std::string path) {
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Não foi possivel iniciar a imagem %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        //convertendo a surface para formato de tela
        optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
        if (optimizedSurface == NULL) {
            printf("Não foi possivel otimizar a imagem %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else {
            printf("IMAGEM %s CARREGADA COM SUCESSO! \n", path.c_str());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return optimizedSurface;
}

void startImages() {
    startImageFlags();
    menuSurface = loadSurface("Imagens/menu.png");
}

void setMenu() {
    menuStatus = 0;
    SDL_BlitSurface(menuSurface, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
}

void updateMainMenu(SDL_Event &tecla) {
    printf("Menu status mudou de %d ", menuStatus);
    if (menuStatus == 0) {
        switch (tecla.key.keysym.sym) {
            case SDLK_LEFT:
                menuStatus = 4;
            case SDLK_RIGHT:
                menuStatus = 1;
        }
    }
    if (menuStatus == 1) {
        switch (tecla.key.keysym.sym) {
        case SDLK_LEFT:
            menuStatus = 0;
        case SDLK_RIGHT:
            menuStatus = 2;
        }
    }
    if (menuStatus == 2) {
        switch (tecla.key.keysym.sym) {
        case SDLK_LEFT:
            menuStatus = 1;
        case SDLK_RIGHT:
            menuStatus = 3;
        }
    }
    if (menuStatus == 3) {
        switch (tecla.key.keysym.sym) {
        case SDLK_LEFT:
            menuStatus = 2;
        case SDLK_RIGHT:
            menuStatus = 4;
        }
    }
    if (menuStatus == 4) {
        switch (tecla.key.keysym.sym) {
        case SDLK_LEFT:
            menuStatus = 3;
        case SDLK_RIGHT:
            menuStatus = 0;
        }
    }
    printf("para %d. \n", menuStatus);
}

int main(int argc, char *argv[]) {
    startSDL();
    createWindow();
    startImages();
    setMenu();
    bool ficar = true;
    while (ficar) {
        while (SDL_PollEvent(&eventHappened)) {
            if (eventHappened.type == SDL_QUIT) {
                SDL_DestroyWindow(window);
                ficar = false;
                return 0;
            }
            else if(eventHappened.type == SDL_KEYDOWN){
                updateMainMenu(eventHappened);
            }
        }
    }
    SDL_Quit();
	return 0;
}