#ifndef GAME_HEADER_INCLUDED
#define GAME_HEADER_INCLUDED

#include "game_state.h"

struct ActorList;
struct InputDevice;
struct World;
struct Map;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Texture SDL_Texture;

struct Game
{
    char* base_path;
    int quit;
    float elapsed_seconds;
    enum GameState state;
    SDL_Renderer* renderer;
    SDL_Window* window;
    struct InputDevice* input_device;
    struct World* world;
};

void init_game(struct Game* game, int width, int height);
void game_main();
void shutdown_game(struct Game* game);
void load_texture(
    struct Game* game,
    SDL_Texture **texture,
    const char* asset_filename);

#endif