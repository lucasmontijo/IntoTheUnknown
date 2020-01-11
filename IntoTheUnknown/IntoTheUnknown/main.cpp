#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
#include"SDL_image.h"
#include<iostream>
#include<string>
#include"play.h"
#define PLAYING true
#define NUM_SPRITES 5
#define WIDTH 960
#define HEIGHT 540
#define PLAYER_LIVES_VALUE 3

typedef struct PLAYER {
    int lives;
    SDL_Surface* surface[NUM_SPRITES];
    SDL_Texture* texture[NUM_SPRITES];
}PLAYER;

typedef struct TELA {
    SDL_Surface* surface;
    SDL_Texture* texture;
}TELA;


//VARIAVEIS GLOBAIS
int menuStatus;
int gameStatus;
SDL_Rect fundo;
PLAYER* player;

//RECTS
SDL_Rect menuSelectionRect;

//RENDERERS
SDL_Renderer* renderer;

//SURFACES
SDL_Surface* screenSurface;
SDL_Surface* menuSurface;
SDL_Surface* menuSelectionSurface;

//TEXTURES 
SDL_Texture* menuFundoTexture;
SDL_Texture* menuSelectionTexture;

//WINDOWS
SDL_Window* window;

//EVENTS
SDL_Event eventHappened;

//HEADER OF FUNCTIONS BELOW MAIN
bool loadPlayerSurfaces();
void play();
void setNewVoidPlayer();
void setPlayerLives(int lives);
void setPlay();
bool loadPlayerTextures();

//FUNCTIONS
void startSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL not initialized. Error: %s", SDL_GetError());
    }
    IMG_Init(IMG_INIT_PNG);
}

void createWindow() {
    window = SDL_CreateWindow("Into The Unknown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void startImageFlags() {
    int imgFlags = IMG_INIT_PNG;
    if (!IMG_Init(imgFlags) && !imgFlags) {
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

SDL_Texture* newTexture(SDL_Surface* surface) {
    Uint32 colorkey = SDL_MapRGB(surface->format, 0, 0, 0);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);
    return SDL_CreateTextureFromSurface(renderer, surface);
}

void startImages() {
    startImageFlags();
    menuSurface = IMG_Load("Imagens/menu.png");
    menuSelectionSurface = IMG_Load("Imagens/selection.png");
    SDL_SetSurfaceBlendMode(menuSelectionSurface, SDL_BLENDMODE_BLEND);
}

void setMenuPrincipal() {
    menuSelectionRect.w = 163;
    menuSelectionRect.h = 48; 
    menuSelectionRect.x = 40;
    menuSelectionRect.y = 381;
}

void updateMainMenu(int number) {
    printf("Menu status mudou de %d ", menuStatus);
    menuStatus += number;
    if (menuStatus < 0) menuStatus = 4;
    if (menuStatus > 4) menuStatus = 0;
    if (menuStatus == 0) {
        setMenuPrincipal();
    }
    if (menuStatus == 1) {
        menuSelectionRect.x = 218;
        menuSelectionRect.w = 200;
    }
    if (menuStatus == 2) {
        menuSelectionRect.x = 433;
        menuSelectionRect.w = 200;
    }
    if(menuStatus == 3){
        menuSelectionRect.x = 643;
        menuSelectionRect.w = 148;
    }
    if (menuStatus == 4) {
        menuSelectionRect.x = 800;
        menuSelectionRect.w = 110;
    }
    printf("para %d. \n", menuStatus);
}

SDL_Rect newRect(int x, int y, int h, int w) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.h = h;
    rect.w = w;
    return rect;
}

void clearScreen() {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    startSDL();
    createWindow();
    startImages();
    setMenuPrincipal();
    bool stay = true;
    while (stay) {
        while (SDL_PollEvent(&eventHappened)) {
            if (eventHappened.type == SDL_QUIT) {
                SDL_DestroyTexture(menuFundoTexture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                stay = false;
                return 0;
            }
            if (eventHappened.type == SDL_KEYDOWN) {
                if(eventHappened.key.keysym.sym == SDLK_LEFT || eventHappened.key.keysym.sym == SDLK_DOWN) updateMainMenu(-1);
                if(eventHappened.key.keysym.sym == SDLK_RIGHT || eventHappened.key.keysym.sym == SDLK_UP) updateMainMenu(1);
                if (eventHappened.key.keysym.sym == SDLK_RETURN) {
                    if (menuStatus == 0) {//JOGAR
                        gameStatus = PLAYING;
                        stay = false;
                    }
                    if (menuStatus == 1) {
                        //CR�DITOS
                    }
                    if (menuStatus == 2) {
                        //RANKING
                    }
                    if (menuStatus == 3) {
                        //AJUDA
                    }
                    if (menuStatus == 4) {
                        //SAIR
                        SDL_DestroyTexture(menuFundoTexture);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        stay = false;
                        return 0;
                    }
                }
            }
            if (eventHappened.type == SDL_MOUSEBUTTONDOWN) {
                printf("X = %d\n", eventHappened.button.x);
                printf("Y = %d\n", eventHappened.button.y);
            }
        }
        menuFundoTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
        menuSelectionTexture = SDL_CreateTextureFromSurface(renderer, menuSelectionSurface);
        SDL_SetTextureBlendMode(menuSelectionTexture, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(renderer, menuFundoTexture, NULL, NULL);
        SDL_RenderCopy(renderer, menuSelectionTexture, NULL, &menuSelectionRect);
        SDL_DestroyTexture(menuFundoTexture);
        SDL_DestroyTexture(menuSelectionTexture);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderPresent(renderer);
    }
    
    if (gameStatus) {
        play();
    }
	return 0;
}

void play() {
    setPlay();
    bool stay = true;
    while (gameStatus) {
        while (SDL_PollEvent(&eventHappened)) {
            if (eventHappened.type == SDL_QUIT) {
                SDL_DestroyTexture(menuFundoTexture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                stay = false;
            }
        }
    }
}

void setPlay() {
    printf("Jogando");
    clearScreen();
    setNewVoidPlayer();
    setPlayerLives(PLAYER_LIVES_VALUE);
    loadPlayerSurfaces();
    loadPlayerTextures();
}

bool loadPlayerSurfaces() {
    for (int i = 0; i < NUM_SPRITES; i++) {
        std::string index = std::to_string(i);
        std::string final = "Imagens/Personagens/Cientista/player_" + index + ".png";
        player->surface[i] = IMG_Load(final.c_str());
        if (!player->surface[i]) {
            printf("N�o foi possivel carregar imagem %s. Erro %s.\n", final.c_str(), SDL_GetError());
            return false;
        }
        else {
            printf("Imagem player_%d.png carregada com sucesso.\n", i);
        }
    }
    return true;
}

bool loadPlayerTextures() {
    for (int i = 0; i < NUM_SPRITES; i++) {
        player->texture[i] = SDL_CreateTextureFromSurface(renderer, player->surface[i]);
        if (!player->texture[i]) {
            printf("Nao foi possivel carregar a textura para player[%d]. erro %s", i, SDL_GetError());
            return false;
        }
    }
    return true;
}

void setNewVoidPlayer() {
    PLAYER* p = (PLAYER*) malloc(sizeof(PLAYER));
    player = p;
}

void setPlayerLives(int lives) {
    player->lives = lives;
}