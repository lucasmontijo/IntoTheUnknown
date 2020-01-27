#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>
#include<SDL.h>
#include "SDL_opengl.h"
#include"SDL_image.h"
#include<iostream>
#include<string>
#include"play.h"
#include <SDL_ttf.h>
#include <SDL_mixer.h>

//CONSTANTES
#define PLAYING true
#define NUM_SPRITES 9
#define WIDTH 960
#define HEIGHT 540
#define bgWIDTH 1920
#define bgHEIGHT 540
#define PLAYER_LIVES_VALUE 3
#define GRAVITY 10
#define VELOCITY 5
#define MAXJUMPDISTANCE 200
#define ENEMIES_VALUE 3
#define FONTSIZE 25
#define TOOL_BAR_OPACITY 170
#define TIME_MACHINE_SPRITES 6
#define NUM_PLATFORMS 7
#define NUM_STARS 4
#define NUM_LEVELS 4
#define ENEMY_VELOCITY 6
#define MAX_ENEMY_DISTANCE 250
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
	int enemyVelocity;
	SDL_Surface* backgroundSurface;
	SDL_Texture* backgroundTexture;
	SDL_Rect backgroundPosition;
	SDL_Surface* enemySurface;
	SDL_Texture* enemyTexture;
	SDL_Surface* platformSurface;
	SDL_Texture* platform;
	SDL_Surface* starsSurface;
	SDL_Texture* starsTexture;
	SDL_Rect starsPositions[NUM_STARS];
	SDL_Rect platforms[NUM_PLATFORMS];
	SDL_Rect enemies[ENEMIES_VALUE];
	SDL_Rect enemiesStart[ENEMIES_VALUE];
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
	SDL_Rect scorePosition;
	int scoreH;
	int scoreW;
}toolBar;

typedef struct baloon {
	SDL_Texture* texture;
	SDL_Surface* surface;
	SDL_Rect position;
}baloon;

typedef struct timeMachine {
	SDL_Surface* surfaces[TIME_MACHINE_SPRITES];
	SDL_Texture* texture;
	SDL_Rect position;
}timeMachine;

SDL_Rect newRect(int x, int y, int h, int w);
//VARIAVEIS GLOBAIS
bool onInit = false;
int menuStatus;
int scoreTemp = 0;
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
SDL_Surface* textPreSurface;
SDL_Texture* textTexture;
SDL_Texture* textPreTexture;
timeMachine* machine;
Mix_Chunk* jumpSound = NULL;
Mix_Chunk* starSound = NULL;
Mix_Chunk* killSound = NULL;
Mix_Chunk* gameoverSound = NULL;
Mix_Chunk* transitionSound = NULL;
Mix_Chunk* victorySound = NULL;
std::string playerName;
SDL_Rect textRects[1][3] = {
	 {newRect(174, 156, 0, 0), newRect(174, 156, 0, 0), newRect(174, 156, 0, 0)}
};
std::string dialogues[1][3] = {
	{
		"Olá! Meu nome é Dr. Bob, e este é meu laboratório. Nele eu fiz minhas maiores descobertas científicas.\n\n\n Direita -> continuar", 
		"Essa é a minha mais nova invenção: uma máquina do tempo. Infelizmente ainda não calculei todos os parâmetros para seu funcionamento.\n\n Direita -> continuar", 
		"Você poderia me ajudar a ajustar os parâmetros da máquina? \n\n\n\n Direita -> ajudar"
	}
};
std::string levelTitles[NUM_LEVELS] = {
	"Encontre a máquina do tempo coletando estrelas.", 
	"Volte à máquina. Cuidado com inimigos.", 
	"Volte à máquina, Cuidado com os robôs assassinos.",
	"Volte à máquina. Dinossauros são fofos e perigosos!"
};
std::string levelTitlesPre[NUM_LEVELS] = {"Laboratório do Dr. Bob. Atualmente.", 
	"Pântano de Pangéia. 110 milhões de anos atrás.", 
	"Central City. 110 anos no futuro.",
	"Pangéia. 65 milhões de anos atrás."
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
void play(int num);
void setNewVoidPlayer();
void setPlayerLives(int lives);
void setPlay(int num);
bool loadPlayerTexture();
void handleEvent(SDL_Event event);
void setPlayerPosition(int x, int y);
void changePlayerSprite(SDL_Event event);
int getPLayerSpriteStatus();
bool movePlayer();
void loadGroundSurface();
void loadGroundTexture();
void gameOver();
void onDialogue();
void playerLostLife();
void winner();
void gravar(std::string s);

//FUNCTIONS
void startFont();

void startSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL not initialized. Error: %s", SDL_GetError());
	}
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	startFont();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
}

void createWindow() {
	window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void createTimeMachine() {
	timeMachine* a = (timeMachine*)malloc(sizeof(timeMachine));
	machine = a;
	std::string path;
	for (int i = 0; i < TIME_MACHINE_SPRITES; i++) {
		path = "Imagens/TimeMachine/timeMachine_" + std::to_string(i) + ".png";
		machine->surfaces[i] = IMG_Load(path.c_str());
		if (!machine->surfaces[i]) printf("Não foi possivel abrir a imagem %s.\n", path.c_str());
		else printf("Imagem %s carregada com sucesso!\n", path.c_str());
	}
}

int* returnPointer(int num) {
	return &num;
}

void createDialogueBaloon() {
	baloon* a = (baloon*)malloc(sizeof(baloon));
	dialogueBaloon = a;
	dialogueBaloon->surface = IMG_Load("Imagens/baloon.png");
	if (!dialogueBaloon->surface) printf("Não foi possível abrir baloon.png\n");
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
	bar->position.h = 70;
	bar->position.w = WIDTH;
	bar->position.x = 0;
	bar->position.y = 0;

	bar->livesPosition.h = bar->livesSurfaces[0]->h/2;
	bar->livesPosition.w = bar->livesSurfaces[0]->w/2;
	bar->livesPosition.x = 50;
	bar->livesPosition.y = (bar->position.h / 2) - (bar->livesPosition.h / 2);

	std::string c = "Score: " + std::to_string(player->score);
	bar->scoreSurface = TTF_RenderText_Blended_Wrapped(font, c.c_str(), WHITE, 200);
	bar->scorePosition = newRect(731, bar->livesPosition.y + 40, bar->scoreSurface->h, bar->scoreSurface->w);
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
	if (!font) printf("Não foi possível abrir fonte.ttf");
	else printf("fonte.ttf carregada com sucesso! \n");
}

int main(int argc, char *argv[]) {
	startSDL();
	createWindow();
	startImages();
	setMenuPrincipal();
	jumpSound = Mix_LoadWAV("sounds/jump.wav");
	starSound = Mix_LoadWAV("sounds/star.wav");
	killSound = Mix_LoadWAV("sounds/kill.wav");
	gameoverSound = Mix_LoadWAV("sounds/gameover.wav");
	transitionSound = Mix_LoadWAV("sounds/transition.wav");
	victorySound = Mix_LoadWAV("sounds/victory.wav");
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
						onInit = true;
						stay = false;
					}
					if (menuStatus == 1) {
						//CRÉDITOS
						SDL_RenderClear(renderer);
						SDL_Rect recta = newRect(0, 0, HEIGHT, WIDTH);
						SDL_Texture* creditos = SDL_CreateTextureFromSurface(renderer, IMG_Load("Imagens/creditos.png"));
						SDL_RenderCopy(renderer, creditos, NULL, &recta);
						SDL_RenderPresent(renderer);
						SDL_DestroyTexture(creditos);
						SDL_Delay(5000);
					}
					if (menuStatus == 2) {
						//RANKING
						SDL_RenderClear(renderer);
						SDL_Rect rects[5];
						SDL_Surface* surfaces[5];
						SDL_Texture* textures[5];
						SDL_Rect rects2[5];
						SDL_Surface* surfaces2[5];
						SDL_Texture* textures2[5];
						rects[0] = newRect(0, 0, 0, 0);
						rects[1] = newRect(0, 50, 0, 0);
						rects[2] = newRect(0, 100, 0, 0);
						rects[3] = newRect(0, 150, 0, 0);
						rects[4] = newRect(0, 200, 0, 0);
						rects2[0] = newRect(400, 0, 0, 0);
						rects2[1] = newRect(400, 50, 0, 0);
						rects2[2] = newRect(400, 100, 0, 0);
						rects2[3] = newRect(400, 150, 0, 0);
						rects2[4] = newRect(400, 200, 0, 0);
						FILE* a = fopen("Ranking.txt", "r");
						int ranking[5];
						char nomea[5][30];
						char temp[5][6];
						for (int i = 0; i < 5; i++) {
							fgets(nomea[i], 30, a);
							fgets(temp[i], 6, a);
						}
						for (int i = 0; i < 5; i++) {
							surfaces[i] = TTF_RenderText_Solid(font, nomea[i], WHITE);
							textures[i] = SDL_CreateTextureFromSurface(renderer, surfaces[i]);
							SDL_QueryTexture(textures[i], NULL, NULL, &rects[i].w, &rects[i].h);
							SDL_RenderCopy(renderer, textures[i], NULL, &rects[i]);
							SDL_DestroyTexture(textures[i]);
							SDL_FreeSurface(surfaces[i]);
						}
						for (int i = 0; i < 5; i++) {
							surfaces2[i] = TTF_RenderText_Solid(font, temp[i], WHITE);
							textures2[i] = SDL_CreateTextureFromSurface(renderer, surfaces2[i]);
							SDL_QueryTexture(textures2[i], NULL, NULL, &rects2[i].w, &rects2[i].h);
							SDL_RenderCopy(renderer, textures2[i], NULL, &rects2[i]);
							SDL_DestroyTexture(textures2[i]);
							SDL_FreeSurface(surfaces2[i]);
						}
						SDL_RenderPresent(renderer);
						SDL_Delay(5000);
					}
					if (menuStatus == 3) {
						//AJUDA
						SDL_RenderClear(renderer);
						SDL_Texture* ajuda = SDL_CreateTextureFromSurface(renderer, IMG_Load("Imagens/ajuda.png"));
						SDL_RenderCopy(renderer, ajuda, NULL, &newRect(0, 0, HEIGHT, WIDTH));
						SDL_RenderPresent(renderer);
						SDL_DestroyTexture(ajuda);
						SDL_Delay(5000);
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
	int timeMachineStatus = 0;
	createTimeMachine();
	
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
	setPlay(gameLevel->number);
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
		if (dialogueStatus == 0) { //Diálogo Zero, ao fim, play na fase 1 468 138 xy
			if (currentStatus >= dialogueQuant) {
				stay = false;
				currentStatus = 0;
				break;
			}
			//Limpar o renderer
			SDL_RenderClear(renderer);
			
			//criar surfaces
			textSurface = TTF_RenderText_Blended_Wrapped(font, dialogues[0][currentStatus].c_str(), BLACK, 430);
			std::string c = "Score: " + std::to_string(player->score);
			bar->scoreSurface = TTF_RenderText_Blended_Wrapped(font, c.c_str(), WHITE, 200);


			//Criar texturas
			gameLevel->backgroundTexture = SDL_CreateTextureFromSurface(renderer, gameLevel->backgroundSurface);
			bar->background = SDL_CreateTextureFromSurface(renderer, bar->backgroundSurface);
			bar->lives = SDL_CreateTextureFromSurface(renderer, bar->livesSurfaces[player->lives - 1]);
			bar->score = SDL_CreateTextureFromSurface(renderer, bar->scoreSurface);
			SDL_SetTextureAlphaMod(bar->background, TOOL_BAR_OPACITY);
			temporaryPlayerTexture = SDL_CreateTextureFromSurface(renderer, temporatyPlayerSurface);
			dialogueBaloon->texture = SDL_CreateTextureFromSurface(renderer, dialogueBaloon->surface);
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_QueryTexture(textTexture, NULL, NULL, &textRects[dialogueStatus][currentStatus].w, &textRects[dialogueStatus][currentStatus].h);
			SDL_QueryTexture(bar->score, NULL, NULL, &bar->scoreW, &bar->scoreH);

			//Copiar texturas para o renderer
			SDL_RenderCopy(renderer, gameLevel->backgroundTexture, NULL, NULL); //fundo do dialogo
			SDL_RenderCopy(renderer, bar->background, NULL, &bar->position); //fundo da toolBar
			SDL_RenderCopy(renderer, bar->lives, NULL, &bar->livesPosition); //textura das vidas
			SDL_RenderCopy(renderer, bar->score, NULL, &bar->scorePosition); //score
			SDL_RenderCopy(renderer, temporaryPlayerTexture, NULL, &temporaryPlayerRect);//textura apresentação personagem
			SDL_RenderCopy(renderer, dialogueBaloon->texture, NULL, &dialogueBaloon->position);
			SDL_RenderCopy(renderer, textTexture, NULL, &textRects[dialogueStatus][currentStatus]);

			//Exibir máquina do tempo
			if (currentStatus == 1 || currentStatus == 2) {
				machine->position.h = 170;
				machine->position.w = 170;
				machine->position.x = 247;
				machine->position.y = 346;
				machine->texture = SDL_CreateTextureFromSurface(renderer, machine->surfaces[timeMachineStatus]);
				SDL_RenderCopy(renderer, machine->texture, NULL, &machine->position);
				SDL_DestroyTexture(machine->texture);
				if (timeMachineStatus == (TIME_MACHINE_SPRITES - 1)) timeMachineStatus = 0;
				else timeMachineStatus++;
			}

			//Limpar texturas criadas para melhor uso de memória
			SDL_DestroyTexture(gameLevel->backgroundTexture);
			SDL_DestroyTexture(bar->background);
			SDL_DestroyTexture(bar->lives);
			SDL_DestroyTexture(temporaryPlayerTexture);
			SDL_DestroyTexture(dialogueBaloon->texture);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);
			SDL_DestroyTexture(bar->score);
			SDL_FreeSurface(bar->scoreSurface);


			//Apresentar o render com as texturas copiadas
			SDL_RenderPresent(renderer);
		}
	}
	SDL_DestroyTexture(gameLevel->backgroundTexture);
	SDL_DestroyTexture(bar->background);
	SDL_DestroyTexture(bar->lives);
	SDL_DestroyTexture(temporaryPlayerTexture);
	SDL_DestroyTexture(dialogueBaloon->texture);
	SDL_DestroyTexture(textTexture);
	play(dialogueStatus);
}

void play(int num) {
	int currentStatus = 0;
	SDL_Rect tempRect;
	SDL_Rect tempRect2;
	setPlay(num);
	bool stay = true;
	SDL_RenderClear(renderer);
	textSurface = TTF_RenderText_Blended_Wrapped(font, levelTitles[num].c_str(), WHITE, 800);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if(num<NUM_LEVELS)textPreSurface = TTF_RenderText_Solid(font, levelTitlesPre[num].c_str(), WHITE);
	textPreTexture = SDL_CreateTextureFromSurface(renderer, textPreSurface);
	SDL_QueryTexture(textPreTexture, NULL, NULL, &tempRect2.w, &tempRect2.h);
	SDL_QueryTexture(textTexture, NULL, NULL, &tempRect.w, &tempRect.h);
	tempRect2.x = WIDTH / 2 - tempRect2.w / 2;
	tempRect2.y = HEIGHT / 2 - tempRect2.h - 20;
	SDL_RenderCopy(renderer, textPreTexture, NULL, &tempRect2);
	SDL_RenderCopy(renderer, textTexture, NULL, &newRect((WIDTH / 2 - textSurface->w / 2), (HEIGHT / 2 - textSurface->h / 2), tempRect.h, tempRect.w));
	SDL_RenderPresent(renderer);
	SDL_Delay(5000);
	while (stay) {
		while (SDL_PollEvent(&eventHappened)) {
			if (eventHappened.type == SDL_QUIT) {
				SDL_DestroyTexture(player->texture);
				SDL_DestroyTexture(groundTexture);
				SDL_DestroyTexture(bar->background);
				SDL_DestroyTexture(bar->lives);
				SDL_DestroyTexture(machine->texture);
				SDL_DestroyTexture(gameLevel->backgroundTexture);
				SDL_DestroyTexture(gameLevel->starsTexture);
				if (gameLevel->number > 0) SDL_DestroyTexture(gameLevel->enemyTexture);
				SDL_DestroyTexture(bar->score);
				SDL_DestroyTexture(gameLevel->platform);
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				stay = false;
				break;
			}
			if (eventHappened.type == SDL_MOUSEBUTTONDOWN) {
				printf("X = %d\n", eventHappened.button.x);
				printf("Y = %d\n", eventHappened.button.y);
			}
			else handleEvent(eventHappened);
		}
		//limpar o renderer
		SDL_RenderClear(renderer);

		//criar surface do score
		std::string c = "Score: " + std::to_string(player->score);
		bar->scoreSurface = TTF_RenderText_Blended_Wrapped(font, c.c_str(), WHITE, 200);
		
		//chamar função para verificar movimento
		movePlayer();

		//criar texturas
		bar->background = SDL_CreateTextureFromSurface(renderer, bar->backgroundSurface);
		SDL_SetTextureAlphaMod(bar->background, TOOL_BAR_OPACITY);
		bar->lives = SDL_CreateTextureFromSurface(renderer, bar->livesSurfaces[player->lives - 1]);
		gameLevel->backgroundTexture = SDL_CreateTextureFromSurface(renderer, gameLevel->backgroundSurface);
		loadPlayerTexture();
		gameLevel->starsTexture = SDL_CreateTextureFromSurface(renderer, gameLevel->starsSurface);
		machine->texture = SDL_CreateTextureFromSurface(renderer, machine->surfaces[currentStatus]);
		currentStatus++;
		if (currentStatus < 0 || currentStatus>=TIME_MACHINE_SPRITES) currentStatus = 0;
		player->position.h = player->surface[player->spriteStatus]->h;
		player->position.w = player->surface[player->spriteStatus]->w;
		bar->score = SDL_CreateTextureFromSurface(renderer, bar->scoreSurface);
		SDL_QueryTexture(bar->score, NULL, NULL, &bar->scoreW, &bar->scoreH);
		gameLevel->platform = SDL_CreateTextureFromSurface(renderer, gameLevel->platformSurface);

		//copiar textures para o render
		SDL_RenderCopy(renderer, gameLevel->backgroundTexture, NULL, &gameLevel->backgroundPosition);//
		for (int i = 0; i < NUM_PLATFORMS; i++) {
			SDL_RenderCopy(renderer, gameLevel->platform, NULL, &gameLevel->platforms[i]);
		}
		for (int i = 0; i < NUM_STARS; i++) {
			SDL_RenderCopy(renderer, gameLevel->starsTexture, NULL, &gameLevel->starsPositions[i]);
		}
		SDL_RenderCopy(renderer, player->texture, NULL, &player->position);
		SDL_RenderCopy(renderer, machine->texture, NULL, &machine->position);
		if (gameLevel->number > 0) {
			gameLevel->enemyTexture = SDL_CreateTextureFromSurface(renderer, gameLevel->enemySurface);
			for (int i = 0; i < ENEMIES_VALUE; i++) {
				SDL_RenderCopy(renderer, gameLevel->enemyTexture, NULL, &gameLevel->enemies[i]);
			}
			//SDL_DestroyTexture(gameLevel->enemyTexture);
		}
		SDL_RenderCopy(renderer, bar->background, NULL, &bar->position); //fundo da toolBar
		SDL_RenderCopy(renderer, bar->lives, NULL, &bar->livesPosition); //textura das vidas
		SDL_RenderCopy(renderer, bar->score, NULL, &bar->scorePosition);
		
		//limpar texturas
		SDL_DestroyTexture(player->texture); 
		SDL_DestroyTexture(groundTexture);
		SDL_DestroyTexture(bar->background);
		SDL_DestroyTexture(bar->lives);
		SDL_DestroyTexture(machine->texture);
		SDL_DestroyTexture(gameLevel->backgroundTexture);
		SDL_DestroyTexture(gameLevel->starsTexture);
		if(gameLevel->number>0) SDL_DestroyTexture(gameLevel->enemyTexture);
		SDL_DestroyTexture(bar->score);
		SDL_DestroyTexture(gameLevel->platform);

		//apresentar o render
		SDL_RenderPresent(renderer);
		//SDL_Delay(10);
	}
}

int getPLayerSpriteStatus() {
	return player->spriteStatus;
}

void setPlay(int num) {
	printf("Jogando");
	clearScreen();
	if (onInit) {
		onInit = false;
		setNewVoidPlayer();
		player->lives = PLAYER_LIVES_VALUE;
	}
	loadPlayerSurfaces();
	setPlayerPosition(0, 100);
	loadGroundSurface();
	loadGroundTexture();
	jumpStartPosition = newRect(0, 0, 0, 0);
	groundRect = newRect(0, HEIGHT - 100, 100, 2 * WIDTH);
	player->yVel = GRAVITY;
	SDL_RenderPresent(renderer);
	player->jumpStatus = false;
	switch (num) {
		//definir parametros primeira missao
	case 0:
		gameLevel->backgroundSurface = IMG_Load("Imagens/lab_background_full.png");
		gameLevel->backgroundPosition = newRect(0, 0, bgHEIGHT, bgWIDTH);
		gameLevel->platformSurface = IMG_Load("Imagens/ground.png");
		gameLevel->starsSurface = IMG_Load("Imagens/star.png");
		gameLevel->platform = SDL_CreateTextureFromSurface(renderer, gameLevel->platformSurface);
		gameLevel->platforms[0] = newRect(0, HEIGHT - 50, 50, (2 * WIDTH - 500));//chao
		gameLevel->platforms[1] = newRect(500, HEIGHT - 200, 50, gameLevel->platformSurface->w);//plat 1
		gameLevel->platforms[2] = newRect(bgWIDTH - 400, HEIGHT - 200, 50, 300);//plat final
		gameLevel->platforms[3] = newRect(880, 208, 50, 300);//plat 2
		gameLevel->platforms[4] = newRect(0, 0, 0, 0);
		gameLevel->platforms[5] = newRect(0, 0, 0, 0);
		gameLevel->platforms[6] = newRect(0, 0, 0, 0);
		gameLevel->starsPositions[0] = newRect(582, 277, 50, 50);
		gameLevel->starsPositions[1] = newRect(WIDTH + 45, 150, 50, 50);
		gameLevel->starsPositions[2] = newRect(WIDTH + 45, 442, 50, 50);
		machine->position.x = 637 + WIDTH;
		machine->position.y = HEIGHT - gameLevel->platforms[2].y - machine->position.h + 140;
		printf("Case zero");
		break;
	case 1:
		gameLevel->backgroundSurface = IMG_Load("Imagens/fase2.png");
		gameLevel->backgroundPosition = newRect(0, 0, bgHEIGHT, bgWIDTH);
		gameLevel->platformSurface = IMG_Load("Imagens/ground_mission2.png");
		gameLevel->starsSurface = IMG_Load("Imagens/star.png");
		gameLevel->enemySurface = IMG_Load("Imagens/arvore2.png");
		gameLevel->enemyVelocity = ENEMY_VELOCITY;
		gameLevel->enemies[0] = newRect(378, 274, 100, 100);
		gameLevel->enemies[1] = newRect(787, HEIGHT - 50 - gameLevel->enemies[0].h, 100, 100);
		for (int i = 0; i < ENEMIES_VALUE; i++) {
			gameLevel->enemiesStart[i] = gameLevel->enemies[i];
		}
		gameLevel->starsPositions[0] = newRect(859, 156, 50, 50);
		gameLevel->starsPositions[1] = newRect(WIDTH + 424, 90, 50, 50);
		gameLevel->starsPositions[2] = newRect(WIDTH + 118, 435, 50, 50);
		gameLevel->platform = SDL_CreateTextureFromSurface(renderer, gameLevel->platformSurface);
		gameLevel->platforms[0] = newRect(0, HEIGHT - 50, 50, 300);//chao inicial
		gameLevel->platforms[1] = newRect(375, 375, 50, 300);
		gameLevel->platforms[2] = newRect(673, 208, 50, 300);
		gameLevel->platforms[3] = newRect(770, HEIGHT-50, 50, 500);
		gameLevel->platforms[4] = newRect(bgWIDTH - 345, 285, 50, 300);
		gameLevel->platforms[5] = newRect(bgWIDTH - 749, 363, 50, 300);
		gameLevel->platforms[6] = newRect(0, 0, 0, 0);
		machine->position.x = WIDTH + 740;
		machine->position.y = 118;
		break;
	case 2:
		machine->position.x = WIDTH + 740;
		gameLevel->backgroundSurface = IMG_Load("Imagens/future_city.png");
		gameLevel->backgroundPosition = newRect(0, 0, bgHEIGHT, bgWIDTH);
		gameLevel->platformSurface = IMG_Load("Imagens/ground3.png");
		gameLevel->enemySurface = IMG_Load("Imagens/robot2.png");
		gameLevel->starsSurface = IMG_Load("Imagens/star.png");
		gameLevel->platforms[0] = newRect(0, HEIGHT - 50, 50, 300);//chao inicial
		gameLevel->platforms[1] = newRect(367, 116, 50, 300);
		gameLevel->platforms[2] = newRect(468, 426, 50, 300);
		gameLevel->platforms[3] = newRect(906, 357, 50, 300);
		gameLevel->platforms[4] = newRect(610, 250, 50, 100);
		gameLevel->platforms[5] = newRect(WIDTH + 351, 224, 50, 300);
		gameLevel->platforms[6] = newRect(WIDTH + 750, 283, 50, 300);
		gameLevel->starsPositions[0] = newRect(488, 72, 50, 50);
		gameLevel->starsPositions[1] = newRect(WIDTH + 242, 214, 50, 50);
		gameLevel->starsPositions[2] = newRect(WIDTH + 666, 77, 50, 50);
		gameLevel->enemies[0] = newRect(WIDTH + 345, 118, 100, 100);
		gameLevel->enemies[1] = newRect(905, 268, 100, 100);
		gameLevel->enemies[2] = newRect(0, 0, 0, 0);
		//gameLevel->enemies[2] = newRect(460, 328, 100, 100);
		for (int i = 0; i < ENEMIES_VALUE; i++) {
			gameLevel->enemiesStart[i] = gameLevel->enemies[i];
		}
		break;
	case 3:
		machine->position.x = WIDTH + 740;
		gameLevel->backgroundSurface = IMG_Load("Imagens/pangeia.png");
		gameLevel->backgroundPosition = newRect(0, 0, bgHEIGHT, bgWIDTH);
		gameLevel->platformSurface = IMG_Load("Imagens/ground_mission2.png");
		gameLevel->enemySurface = IMG_Load("Imagens/dinossauro2.png");
		gameLevel->starsSurface = IMG_Load("Imagens/star.png");
		gameLevel->platforms[0] = newRect(0, HEIGHT - 50, 50, 300);//chao inicial
		gameLevel->platforms[1] = newRect(517, HEIGHT - 50, 50, 700);//chao inicial
		gameLevel->platforms[2] = newRect(WIDTH + 303, 382, 50, 300);
		gameLevel->platforms[3] = newRect(WIDTH + 26, 206, 50, 200);
		gameLevel->platforms[4] = newRect(640, 206, 50, 150);
		gameLevel->platforms[5] = newRect(WIDTH + 724, 279, 50, 300);
		gameLevel->platforms[6] = newRect(0, 0, 0, 0);
		gameLevel->starsPositions[0] = newRect(697, 153, 50, 50);
		gameLevel->starsPositions[1] = newRect(WIDTH + 533, 328, 50, 50);
		gameLevel->starsPositions[2] = newRect(WIDTH + 92, 153, 50, 50);
		gameLevel->enemies[0] = newRect(600, 400, 100, 100);
		gameLevel->enemies[1] = newRect(600 + MAX_ENEMY_DISTANCE, 400, 100, 100);
		gameLevel->enemies[2] = newRect(WIDTH + 312, 292, 100, 100);
		for (int i = 0; i < ENEMIES_VALUE; i++) {
			gameLevel->enemiesStart[i] = gameLevel->enemies[i];
		}
		break;
	}
}

void handleEvent(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		//movePlayer();
		if (event.key.keysym.sym == SDLK_UP) {
			if (player->jumpStatus == false) {
				Mix_PlayChannel(-1, jumpSound, 0);
				printf("mds pulei");
				player->yVel = -GRAVITY;
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
			player->xVel = -(VELOCITY);
		}
		if (event.key.keysym.sym == SDLK_RIGHT) {
			//PARA DIREITA
			changePlayerSprite(event);
			player->xVel = VELOCITY;
		}
		if (event.key.keysym.sym == SDLK_SPACE) {
			//ESPAÇO
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

bool movePlayer() {
	int difference = 0;
	bool hasCollision = false;
	bool hasXCollision = false;
	bool hasYCollision = false;

	//calcular a projeções do personagem com a velocidade atual
	SDL_Rect playerProjection = newRect(player->position.x + player->xVel, player->position.y + player->yVel, player->position.h, player->position.w);
	if (SDL_RectEquals(&player->position, &playerProjection)) return false;
	SDL_Rect playerYProjection = newRect(player->position.x, playerProjection.y, player->position.h, player->position.w);
	SDL_Rect playerXProjection = newRect(playerProjection.x, player->position.y, player->position.h, player->position.w);

	//verificar se personagem colide com as plataformas do nível
	for (int i = 0; i < NUM_PLATFORMS; i++) {
		if (SDL_HasIntersection(&playerProjection, &gameLevel->platforms[i])) {
			hasCollision = true;
			if (SDL_HasIntersection(&playerXProjection, &gameLevel->platforms[i])) hasXCollision = true;
			if (SDL_HasIntersection(&playerYProjection, &gameLevel->platforms[i])) hasYCollision = true;
		}
	}

	//Se o jogador colidir com algo na horizontal
	if (hasXCollision) {
		player->xVel = 0;
		playerProjection.x = player->position.x;
	}

	//Se o jogador colidir com algo na vertical
	if (hasYCollision) {
		player->jumpStatus = false;
		player->yVel = GRAVITY;
		playerProjection.y = player->position.y;
	}

	//Se atingiu o limite maximo de altura para pulo
	if (player->jumpStatus == true && jumpStartPosition.y - player->position.y >= MAXJUMPDISTANCE) {
		player->yVel = GRAVITY;
	}

	//mover background e rects
	difference = playerProjection.x - player->position.x;
	if (playerProjection.x < (WIDTH * 0.3) && playerProjection.x > 0) { //player na primeira parte da tela
		if (difference < 0) {//personagem se movendo p tras
			if (gameLevel->backgroundPosition.x - difference <= 0) {//se da pra mover o fundo
				gameLevel->backgroundPosition.x -= difference;
				//mover tb rects
				machine->position.x -= difference;
				for (int i = 0; i < NUM_PLATFORMS; i++) {
					gameLevel->platforms[i].x -= difference;
				}
				for (int i = 0; i < NUM_STARS; i++) {
					gameLevel->starsPositions[i].x -= difference;
				}
				for (int i = 0; i < ENEMIES_VALUE; i++) {
					gameLevel->enemiesStart[i].x -= difference;
					gameLevel->enemies[i].x -= difference;
				}
			}
			else { //se n da pra mover bg
				gameLevel->backgroundPosition.x = 0;
				player->position.x = playerProjection.x;
			}
		}
		else if (difference > 0) {//personagem movendo p frente
			player->position.x = playerProjection.x;
		}
	}
	else if (playerProjection.x > ((0.6) * WIDTH) && playerProjection.x <= WIDTH-player->position.w) {//personagem na ultima parte da tela
		if (difference > 0) {//personagem se movendo p frente
			if (gameLevel->backgroundPosition.x - difference >= -WIDTH) {//pode mover o bg
				gameLevel->backgroundPosition.x -= difference;
				//fazer rects moverem
				machine->position.x -= difference;
				for (int i = 0; i < NUM_PLATFORMS; i++) {
					gameLevel->platforms[i].x -= difference;
				}
				for (int i = 0; i < NUM_STARS; i++) {
					gameLevel->starsPositions[i].x -= difference;
				}
				for (int i = 0; i < ENEMIES_VALUE; i++) {
					gameLevel->enemiesStart[i].x -= difference;
					gameLevel->enemies[i].x -= difference;
				}
			}
			else {//n da pra mover o bg
				gameLevel->backgroundPosition.x = -WIDTH;
				player->position.x = playerProjection.x;
			}
		}
		if (difference < 0) {
			player->position.x = playerProjection.x;
		}
	}
	else {
		player->position.x = playerProjection.x;
	}
	if (player->position.x < 0) player->position.x = 0;
	if (player->position.x >= WIDTH - player->position.w) player->position.x = WIDTH - player->position.w;
	player->position.y = playerProjection.y;

	//verificar se player caiu em abismos
	if (player->position.y > HEIGHT) playerLostLife();

	//verificar pontuação
	for (int i = 0; i < NUM_STARS; i++) {
		if (SDL_HasIntersection(&player->position, &gameLevel->starsPositions[i])) {
			Mix_PlayChannel(-1, starSound, 0);
			gameLevel->starsPositions[i] = newRect(0, 0, 0, 0);
			player->score += 300;
		}
	}

	//verificar se encerrou o nível
	if (SDL_HasIntersection(&player->position, &machine->position)) {
		if (gameLevel->number + 1 >= 0 && gameLevel->number + 1 < NUM_LEVELS) {
			Mix_PlayChannel(-1, transitionSound, 0);
			gameLevel->number++;
			scoreTemp = player->score;
			play(gameLevel->number);
		}
		else{
			winner();
		}
	}

	//mover inimigo
	if(gameLevel->number > 0) for (int i = 0; i < ENEMIES_VALUE; i++) {
		if (gameLevel->enemyVelocity > 0) {
			if (gameLevel->enemies[i].x + gameLevel->enemyVelocity - gameLevel->enemiesStart[i].x <= MAX_ENEMY_DISTANCE) {
				gameLevel->enemies[i].x += gameLevel->enemyVelocity;
			}
			else {
				gameLevel->enemyVelocity *= -1;
				SDL_FreeSurface(gameLevel->enemySurface);
				if (gameLevel->number == 1) gameLevel->enemySurface = IMG_Load("Imagens/arvore.png");
				if (gameLevel->number == 2) gameLevel->enemySurface = IMG_Load("Imagens/robot.png");
				if (gameLevel->number == 3) gameLevel->enemySurface = IMG_Load("Imagens/dinossauro.png");
			}
		}
		else {
			if (gameLevel->enemies[i].x + gameLevel->enemyVelocity >= gameLevel->enemiesStart[i].x) {
				gameLevel->enemies[i].x += gameLevel->enemyVelocity;
			}
			else {
				gameLevel->enemyVelocity *= -1;
				SDL_FreeSurface(gameLevel->enemySurface);
				if(gameLevel->number == 1) gameLevel->enemySurface = IMG_Load("Imagens/arvore2.png");
				if (gameLevel->number == 2) gameLevel->enemySurface = IMG_Load("Imagens/robot2.png");
				if (gameLevel->number == 3) gameLevel->enemySurface = IMG_Load("Imagens/dinossauro2.png");
			}
		}
	}

	//verificar colisao com inimigo
	for (int i = 0; i < ENEMIES_VALUE; i++) {
		if (SDL_HasIntersection(&playerYProjection, &gameLevel->enemies[i]) && !SDL_HasIntersection(&playerXProjection, &gameLevel->enemies[i])) {
			gameLevel->enemies[i] = newRect(0, 0, 0, 0);
			Mix_PlayChannel(-1, killSound, 0);
			player->score += 100;
		}
		else if (SDL_HasIntersection(&playerXProjection, &gameLevel->enemies[i])) {
			playerLostLife();
		}
	}
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
	//SDL_DestroyTexture(player->texture);
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
			printf("Não foi possivel carregar imagem %s. Erro %s.\n", final.c_str(), SDL_GetError());
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
	free(player);
	PLAYER* p = (PLAYER*) malloc(sizeof(PLAYER));
	player = p;
	player->spriteStatus = 0;
	player->score = 0;
	player->xVel = 0;
	player->yVel = GRAVITY;
	player->jumpStatus = false;
	player->lives = PLAYER_LIVES_VALUE;
}

void setPlayerLives(int lives) {
	if (lives >= 0 && lives <= PLAYER_LIVES_VALUE) {
		player->lives = lives;
	}
	if (player->lives == 0) gameOver();
}

void playerLostLife() {
	if (player->lives - 1 > 0) {
		player->lives -= 1;
		player->score = scoreTemp;
		play(gameLevel->number);
	}
	else gameOver();
}

void loadGroundSurface() {
	groundSurface = IMG_Load("Imagens/ground.png");
	printf("Imagem ground.png carregada \n");
}

void loadGroundTexture() {
	groundTexture = SDL_CreateTextureFromSurface(renderer, groundSurface);
}

void gameOver() {
	Mix_PlayChannel(-1, gameoverSound, 0);
	SDL_StartTextInput();
	std::string in;
	bool running = true;
	SDL_Texture* textura;
	SDL_Texture* textura2;
	SDL_Rect temp;
	SDL_Rect temp2;

	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_TEXTINPUT) {
				in += ev.text.text;
			}
			else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE && in.size()) {
				in.pop_back();
			} else if (ev.type == SDL_QUIT) {
				running = false;
			}
			else if (ev.key.keysym.sym == SDLK_KP_ENTER || ev.key.keysym.sym == SDLK_RETURN) {
				gravar(in);
				running = false;
			}
		}
		SDL_RenderClear(renderer);
		std::string a = "Game over. Score: " + std::to_string(player->score) + ". Digite seu nome: ";
		textSurface = TTF_RenderText_Solid(font, a.c_str(), WHITE);
		textura = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_QueryTexture(textura, NULL, NULL, &temp.w, &temp.h);
		temp = newRect(WIDTH * 0.5 - temp.w, HEIGHT * 0.5 - temp.h, temp.h, temp.w);

		textSurface = TTF_RenderText_Solid(font, in.c_str(), WHITE);
		textura2 = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_QueryTexture(textura2, NULL, NULL, &temp2.w, &temp2.h);
		temp2 = newRect(WIDTH * 0.5 - temp.w, HEIGHT * 0.5 - temp2.h + 200, temp2.h, temp2.w);

		SDL_RenderCopy(renderer, textura, NULL, &temp);
		SDL_RenderCopy(renderer, textura2, NULL, &temp2);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(textura);
		SDL_DestroyTexture(textura2);
	}

	SDL_StopTextInput();

	SDL_DestroyWindow(window);
	main(NULL, NULL);
}

void winner() {
	Mix_PlayChannel(-1, victorySound, 0);
	SDL_StartTextInput();
	std::string in;
	bool running = true;
	SDL_Texture* textura;
	SDL_Texture* textura2;
	SDL_Rect temp;
	SDL_Rect temp2;

	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_TEXTINPUT) {
				in += ev.text.text;
			}
			else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_BACKSPACE && in.size()) {
				in.pop_back();
			}
			else if (ev.type == SDL_QUIT) {
				running = false;
			}
			else if (ev.key.keysym.sym == SDLK_KP_ENTER || ev.key.keysym.sym == SDLK_RETURN) {
				gravar(in);
				running = false;
			}
		}
		SDL_RenderClear(renderer);
		std::string a = "Parabéns! Você ganhou!. Score: " + std::to_string(player->score) + ". Digite seu nome: ";
		textSurface = TTF_RenderText_Solid(font, a.c_str(), WHITE);
		textura = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_QueryTexture(textura, NULL, NULL, &temp.w, &temp.h);
		temp = newRect(WIDTH * 0.5 - temp.w, HEIGHT * 0.5 - temp.h, temp.h, temp.w);

		textSurface = TTF_RenderText_Solid(font, in.c_str(), WHITE);
		textura2 = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_QueryTexture(textura2, NULL, NULL, &temp2.w, &temp2.h);
		temp2 = newRect(WIDTH * 0.5 - temp2.w, HEIGHT * 0.5 - temp2.h + 200, temp2.h, temp2.w);

		SDL_RenderCopy(renderer, textura, NULL, &temp);
		SDL_RenderCopy(renderer, textura2, NULL, &temp2);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(textura);
		SDL_DestroyTexture(textura2);
	}
	SDL_StopTextInput();

	SDL_DestroyWindow(window);
	main(NULL, NULL);
}

void gravar(std::string nome) {
	FILE* a = fopen("Ranking.txt", "w");
	int ranking[5];
	char nomea[5][30];
	char playernome[30];
	strcpy(playernome, nome.c_str());
	char temporario[6];
	char temps[30];
	for (int i = 0; i < 5; i++) {
		fgets(nomea[i], 30, a);
		fgets(temporario, 6, a);
		ranking[i] = atoi(temporario);
	}

	freopen("Ranking.txt", "w", a);
	for (int i = 0; i < 5; i++) {
		if (player->score < ranking[i]) {
			int temp = player->score;
			player->score = ranking[i];
			ranking[i] = temp;
			strcpy(temps, *nomea);
			strcpy(*nomea, playernome);
			strcpy(playernome, temps);
		}
	}

	for (int i = 0; i < 5; i++) {
		fprintf(a, std::strcat(nomea[i], "\n"));
		std::string poa = std::to_string(ranking[i]);
		poa += "\n";
		fprintf(a, poa.c_str());
	}
}