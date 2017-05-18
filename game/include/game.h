#ifndef GAME_HEADER_INCLUDED
#define GAME_HEADER_INCLUDED

#include "game_state.h"

struct InputDevice;
struct World;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Texture SDL_Texture;

struct Game
{
    char* BasePath;
    int Quit;
    float ElapsedSeconds;
    enum GameState State;
    SDL_Renderer* Renderer;
    SDL_Window* Window;
    struct InputDevice* InputDevice;
    struct World* World;
};

void GameInit(struct Game* game, int width, int height);
void GameMain();
void GameShutdown(struct Game* game);
void LoadTexture(struct Game* game, SDL_Texture **texture, const char* assetFilename);

#endif