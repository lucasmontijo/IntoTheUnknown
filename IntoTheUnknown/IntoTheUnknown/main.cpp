#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
#include"SDL_image.h"
#include<iostream>
#include<string>
#include"play.h"
#include <SDL_ttf.h>

//CONSTANTES
#define PLAYING true
#define NUM_SPRITES 9
#define WIDTH 960
#define HEIGHT 540
#define PLAYER_LIVES_VALUE 3
#define GRAVITY 30
#define VELOCITY 10
#define MAXJUMPDISTANCE 70
#define ENEMIES_VALUE 5
#define FONTSIZE 25
#define TOOL_BAR_OPACITY 170
SDL_Color WHITE = { 255, 255, 255 };
SDL_Color BLACK = { 0, 0, 0 };


//ESTRUTURAS
typedef struct PLAYER {
	int lives;
	int score;
	SDL_Surface* surface[NUM_SPRITES];
	SDL_Texture* texture;
	SDL_Rect position;
	int xVel;
	int yVel;
	int spriteStatus;
	bool jumpStatus;
}PLAYER;

typedef struct TELA {
	SDL_Surface* surface;
	SDL_Texture* texture;
}TELA;

typedef struct Level {
	int number;
	SDL_Surface* backgroundSurface;
	SDL_Texture* backgroundTexture;
	SDL_Surface* enemySurface;
	SDL_Texture* enemyTexture;
	SDL_Rect enemies[ENEMIES_VALUE];
}Level;

typedef struct toolBar {
	SDL_Surface* backgroundSurface;
	SDL_Surface* livesSurfaces[PLAYER_LIVES_VALUE];
	SDL_Rect livesPosition;
	SDL_Surface* scoreSurface;
	SDL_Texture* lives;
	SDL_Texture* score;
	SDL_Texture* background;
	SDL_Rect position;
}toolBar;

typedef struct baloon {
	SDL_Texture* texture;
	SDL_Surface* surface;
	SDL_Rect position;
}baloon;

SDL_Rect newRect(int x, int y, int h, int w);
//VARIAVEIS GLOBAIS
int menuStatus;
int gameStatus;
SDL_Rect fundo;
PLAYER* player;
int playerStatus;
SDL_Rect jumpStartPosition;
TTF_Font* font;
toolBar* bar;
Level* gameLevel;
int dialogueStatus;
baloon* dialogueBaloon;
SDL_Surface* textSurface;
SDL_Texture* textTexture;
SDL_Rect textRects[1][3] = {
	 {newRect(174, 156, 0, 0), NULL, NULL}
};
std::string dialogues[1][3] = {
	{"Ol�! Meu nome � Dr. Bob. \n Este � meu laborat�rio.", "b", "c"}
};

//RECTS
SDL_Rect menuSelectionRect;
SDL_Rect groundRect;
SDL_Rect temporaryPlayerRect;

//RENDERERS
SDL_Renderer* renderer;

//SURFACES
SDL_Surface* screenSurface;
SDL_Surface* menuSurface;
SDL_Surface* menuSelectionSurface;
SDL_Surface* groundSurface;
SDL_Surface* temporatyPlayerSurface;

//TEXTURES 
SDL_Texture* menuFundoTexture;
SDL_Texture* menuSelectionTexture;
SDL_Texture* groundTexture;
SDL_Texture* temporaryPlayerTexture;

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
bool loadPlayerTexture();
void handleEvent(SDL_Event event);
void setPlayerPosition(int x, int y);
void changePlayerSprite(SDL_Event event);
int getPLayerSpriteStatus();
void movePlayer();
void loadGroundSurface();
void loadGroundTexture();
void gameOver();
void onDialogue();

//FUNCTIONS
void startFont();

void startSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL not initialized. Error: %s", SDL_GetError());
	}
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	startFont();
}

void createWindow() {
	window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void createDialogueBaloon() {
	baloon* a = (baloon*)malloc(sizeof(baloon));
	dialogueBaloon = a;
	dialogueBaloon->surface = IMG_Load("Imagens/baloon.png");
	if (!dialogueBaloon->surface) printf("N�o foi poss�vel abrir baloon.png\n");
	else printf("Imagem baloon.png carregada com sucesso.\n");
	dialogueBaloon->position.w = dialogueBaloon->surface->w-80;
	dialogueBaloon->position.h = dialogueBaloon->surface->h-100;
	dialogueBaloon->position.x = 130;
	dialogueBaloon->position.y = 101;
}

void createGameLevel(int number, SDL_Surface* surface) {
	Level* a = (Level*) malloc(sizeof(Level));
	gameLevel = a;
	gameLevel->number = number;
	gameLevel->backgroundSurface = surface;
}

void setToolBar() {
	toolBar* tools = (toolBar*) malloc(sizeof(toolBar));
	bar = tools;
	std::string a;
	std::string b;
	for (int i = 0; i < PLAYER_LIVES_VALUE; i++) {
		a = std::to_string(i+1);
		b = "Imagens/lives_" + a + ".png";
		bar->livesSurfaces[i] = IMG_Load(b.c_str());
		if (bar->livesSurfaces[i]) printf("Imagem %s aberta com sucesso. \n", b.c_str());
	}
	bar->backgroundSurface = IMG_Load("Imagens/toolBar.png");
	bar->position.h = 100;
	bar->position.w = WIDTH;
	bar->position.x = 0;
	bar->position.y = 0;

	bar->livesPosition.h = bar->livesSurfaces[0]->h;
	bar->livesPosition.w = bar->livesSurfaces[0]->w;
	bar->livesPosition.x = 50;
	bar->livesPosition.y = (bar->position.h / 2) - (bar->livesPosition.h / 2);
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

void startFont() {
	font = TTF_OpenFont("Fontes/fonte.ttf", FONTSIZE);
	if (!font) printf("N�o foi poss�vel abrir fonte.ttf");
	else printf("fonte.ttf carregada com sucesso! \n");
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
				TTF_Quit();
				SDL_Quit();
				return 0;
			}
			if (eventHappened.type == SDL_KEYDOWN) {
				if(eventHappened.key.keysym.sym == SDLK_LEFT || eventHappened.key.keysym.sym == SDLK_DOWN) updateMainMenu(-1);
				if(eventHappened.key.keysym.sym == SDLK_RIGHT || eventHappened.key.keysym.sym == SDLK_UP) updateMainMenu(1);
				if (eventHappened.key.keysym.sym == SDLK_RETURN || eventHappened.key.keysym.sym == SDLK_KP_ENTER) {
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
	if (gameStatus) { //jogador selecionou o jogar no menu
		//play();
		createGameLevel(0, IMG_Load("Imagens/lab_fundo.png"));
		dialogueStatus = 0;
		onDialogue();
	}
	return 0;
}

void onDialogue() {
	int dialogueQuant;
	int currentStatus = 0;
	switch (dialogueStatus) { //mudar o dialogue quant de acordo com qual dialogo for e quantos sub dalogos tem em cada dialogo
		case 0: 
			dialogueQuant = 3;
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			dialogueQuant = 0;
			break;
	}
	setPlay();
	setToolBar();
	createDialogueBaloon();
	temporatyPlayerSurface = IMG_Load("Imagens/Personagens/Cientista/sprite_hd.png");
	temporaryPlayerRect = newRect(468, 268, 514, 514);
	bool stay = true;
	while (stay) {
		while (SDL_PollEvent(&eventHappened)) {
			if (eventHappened.type == SDL_QUIT) {
				SDL_DestroyTexture(gameLevel->backgroundTexture);
				SDL_DestroyWindow(window);
				SDL_Quit();
				stay = false;
			}
			if (eventHappened.type == SDL_MOUSEBUTTONDOWN) {
				printf("X = %d\n", eventHappened.button.x);
				printf("Y = %d\n", eventHappened.button.y);
			}
			if (eventHappened.type == SDL_KEYDOWN) {
				if (eventHappened.key.keysym.sym == SDLK_RIGHT) {
					currentStatus++;
				}
				if (eventHappened.key.keysym.sym == SDLK_LEFT) {
					if (currentStatus > 0) currentStatus--;
				}
			}
		}
		if (dialogueStatus == 0) { //Di�logo Zero, ao fim, play na fase 1 468 138 xy
			if (currentStatus > dialogueQuant) {
				stay = false;
				currentStatus = 0;
				break;
			}

			//Limpar o renderer
			SDL_RenderClear(renderer);

			//criar surfaces
			textSurface = TTF_RenderText_Solid(font, dialogues[0][currentStatus].c_str(), BLACK);

			//Criar texturas
			gameLevel->backgroundTexture = SDL_CreateTextureFromSurface(renderer, gameLevel->backgroundSurface);
			bar->background = SDL_CreateTextureFromSurface(renderer, bar->backgroundSurface);
			bar->lives = SDL_CreateTextureFromSurface(renderer, bar->livesSurfaces[player->lives - 1]);
			SDL_SetTextureAlphaMod(bar->background, TOOL_BAR_OPACITY);
			temporaryPlayerTexture = SDL_CreateTextureFromSurface(renderer, temporatyPlayerSurface);
			dialogueBaloon->texture = SDL_CreateTextureFromSurface(renderer, dialogueBaloon->surface);
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_QueryTexture(textTexture, NULL, NULL, &textRects[dialogueStatus][currentStatus].w, &textRects[dialogueStatus][currentStatus].h);

			//Copiar texturas para o renderer
			SDL_RenderCopy(renderer, gameLevel->backgroundTexture, NULL, NULL); //fundo do dialogo
			SDL_RenderCopy(renderer, bar->background, NULL, &bar->position); //fundo da toolBar
			SDL_RenderCopy(renderer, bar->lives, NULL, &bar->livesPosition); //textura das vidas
			SDL_RenderCopy(renderer, temporaryPlayerTexture, NULL, &temporaryPlayerRect);//textura apresenta��o personagem
			SDL_RenderCopy(renderer, dialogueBaloon->texture, NULL, &dialogueBaloon->position);
			SDL_RenderCopy(renderer, textTexture, NULL, &textRects[dialogueStatus][currentStatus]);

			//Limpar texturas criadas para melhor uso de mem�ria
			SDL_DestroyTexture(gameLevel->backgroundTexture);
			SDL_DestroyTexture(bar->background);
			SDL_DestroyTexture(bar->lives);
			SDL_DestroyTexture(temporaryPlayerTexture);
			SDL_DestroyTexture(dialogueBaloon->texture);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);

			//Apresentar o render com as texturas copiadas
			SDL_RenderPresent(renderer);
		}
	}
	play();
}

void play() {
	//setPlay();
	bool stay = true;
	while (gameStatus) {
		while (SDL_PollEvent(&eventHappened)) {
			if (eventHappened.type == SDL_QUIT) {
				SDL_DestroyTexture(menuFundoTexture);
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				stay = false;
				break;
			}
			else handleEvent(eventHappened);
		}
		SDL_RenderClear(renderer);
		movePlayer();
		player->position.h = player->surface[player->spriteStatus]->h;
		player->position.w = player->surface[player->spriteStatus]->w;
		SDL_RenderCopy(renderer, player->texture, NULL, &player->position);
		SDL_RenderCopy(renderer, groundTexture, NULL, &groundRect);
		SDL_RenderPresent(renderer);
		SDL_Delay(30);
	}
}

int getPLayerSpriteStatus() {
	return player->spriteStatus;
}

void setPlay() {
	printf("Jogando");
	clearScreen();
	setNewVoidPlayer();
	setPlayerLives(PLAYER_LIVES_VALUE);
	loadPlayerSurfaces();
	loadPlayerTexture();
	setPlayerPosition(0, 100);
	loadGroundSurface();
	loadGroundTexture();
	groundRect = newRect(0, HEIGHT - 100, 100, WIDTH);
	player->yVel = GRAVITY;
	SDL_RenderPresent(renderer);
}

void handleEvent(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		movePlayer();
		if (event.key.keysym.sym == SDLK_UP) {
			player->yVel = -GRAVITY;
			if (player->jumpStatus == false) {
				player->jumpStatus = true;
				jumpStartPosition = player->position;
			}
		}
		if (event.key.keysym.sym == SDLK_DOWN) {
			//PARA BAIXO
			player->yVel = GRAVITY;
		}
		if (event.key.keysym.sym == SDLK_LEFT) {
			//PARA ESQUERDA
			changePlayerSprite(event);
			loadPlayerTexture();
			player->xVel = -(VELOCITY);
		}
		if (event.key.keysym.sym == SDLK_RIGHT) {
			//PARA DIREITA
			changePlayerSprite(event);
			loadPlayerTexture();
			player->xVel = VELOCITY;
		}
		if (event.key.keysym.sym == SDLK_SPACE) {
			//ESPA�O
		}
	}
	if (event.type == SDL_KEYUP) {
		if (event.key.keysym.sym == SDLK_UP) {
			player->jumpStatus = false;
			player->yVel = GRAVITY;
		}
		if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)player->xVel = 0;
		
		changePlayerSprite(event);
		loadPlayerTexture();
	}
}

void movePlayer() {
	//calcular a proje��o do personagem com a velocidade atual
	SDL_Rect playerProjection = newRect(player->position.x + player->xVel, player->position.y + player->yVel, player->position.h, player->position.w);


	//mover o personagem somente se a proje��o dele n�o tiver colis�o com o ch�o
	if (!SDL_HasIntersection(&playerProjection, &groundRect)) {
		player->position.y = playerProjection.y;
	}
	else {
		player->yVel = GRAVITY;
	}
	player->position.x = playerProjection.x;
	
	//TODO: if para verificar se personagem e inimigo colidiram.
}

void changePlayerSprite(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) { //andando para esquerda -> imagens 5, 6, 7 e 8
			if (player->spriteStatus < 5 || player->spriteStatus > 8) {
				player->spriteStatus = 5;
			}
			else {
				if (player->spriteStatus != 8) {
					player->spriteStatus++;
				}
				if (player->spriteStatus == 8) {
					player->spriteStatus = 6;
				}
			}
		}
		if (event.key.keysym.sym == SDLK_RIGHT) {//andando para direita -> imagens 1, 2, 3 e 4 
			if (player->spriteStatus < 1 || player->spriteStatus > 4) {
				player->spriteStatus = 1;
			}
			else {
				if (player->spriteStatus != 4) {
					player->spriteStatus++;
				}
				if (player->spriteStatus == 4) {
					player->spriteStatus = 1;
				}
			}
		}
	}
	else if (event.type == SDL_KEYUP && (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)) {
		player->spriteStatus = 0;
	}
	printf("%d\n", player->spriteStatus);
	SDL_DestroyTexture(player->texture);
}

void setPlayerPosition(int x, int y) {
	player->position.x = x;
	player->position.y = y;
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

bool loadPlayerTexture() {
	player->texture = SDL_CreateTextureFromSurface(renderer, player->surface[player->spriteStatus]);
	if (player->texture) return true;
	else return false;
}

void setNewVoidPlayer() {
	PLAYER* p = (PLAYER*) malloc(sizeof(PLAYER));
	player = p;
	player->spriteStatus = 0;
	player->xVel = 0;
	player->yVel = GRAVITY;
	player->jumpStatus = false;
}

void setPlayerLives(int lives) {
	if(lives >= 0 && lives <= PLAYER_LIVES_VALUE) player->lives = lives;
	if (player->lives == 0) gameOver();
}

void loadGroundSurface() {
	groundSurface = IMG_Load("Imagens/ground.png");
	printf("Imagem ground.png carregada \n");
}

void loadGroundTexture() {
	groundTexture = SDL_CreateTextureFromSurface(renderer, groundSurface);
}

void gameOver() {

}