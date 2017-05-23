#include "game.h"
#include "actor.h"
#include "audio.h"
#include "game_state.h"
#include "game_state_level.h"
#include "game_state_inventory.h"
#include "game_state_transition.h"
#include "input.h"
#include "map.h"
#include "map_link.h"
#include "map_load_util.h"
#include "paths.h"
#include "render.h"
#include "tileset.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void LoadMap(
    struct Game* game, 
    const char* assetFilename,
    struct Map** map,
    struct ActorList* actors)
{
    assert(*map == NULL);
    assert(assetFilename != NULL);

    char *fullpath = CreateMapPath(game->BasePath, assetFilename);
    printf("%s\n", fullpath);
    FILE *file = fopen(fullpath, "rb");
    assert(file != NULL);
    free(fullpath);
    *map = LoadMapFromFile(file);
    LoadActorsFromFile(file, *map, actors);
    fclose(file);
}

//  ---------------------------------------------------------------------------
void GameInit(struct Game* game, int width, int height)
{
    assert(game != NULL);
    assert(width > 0);
    assert(height > 0);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
		printf("SDL failed to initialize: %sn", SDL_GetError());
		exit(1);
	}

    game->BasePath = SDL_GetBasePath();
    if (game->BasePath == NULL)
    {
        game->BasePath = SDL_strdup("./");
    }

    if (AudioInit(game) != 0)
    {
  		printf("SDL2_mixer failed to initialize: %sn", Mix_GetError());
		exit(1);      
    }

    game->Window = SDL_CreateWindow(
        "Game", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (game->Window == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    game->Renderer = SDL_CreateRenderer(game->Window, -1, SDL_RENDERER_ACCELERATED);

    if (game->Renderer == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    //  Clear window
    SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->Renderer);
    SDL_RenderPresent(game->Renderer);

    game->ElapsedSeconds = 0;
    game->Quit = 0;
    game->State = GAME_STATE_LEVEL;

    IMG_Init(IMG_INIT_PNG);
}

//  ---------------------------------------------------------------------------
void GameMain()
{
    //  Use same seed everytime for now
    srand(0);

    struct Game game = {0};

    GameInit(&game, 800, 800);

    if (GfxInit(&game) != 0)
    {
        printf("Failed to initialize graphics.\n");
        GameShutdown(&game);
    }

    game.World = CreateWorld();
    LoadMap(&game, "map01", &game.World->Map, game.World->Actors);

    struct InputDevice inputDevice = {0};
    game.InputDevice = &inputDevice;

    struct Input input;

    CreatePlayerActor(game.World);

    unsigned int lastTicks = 0;
    unsigned int ticks = 0;
    while (game.Quit == 0)
    {
        lastTicks = ticks;
        ticks = SDL_GetTicks();
        game.ElapsedSeconds = (ticks - lastTicks) / 1000.0f;

        UpdateInput(&input, game.InputDevice);
        game.Quit = game.InputDevice->Quit;
        
        switch (game.State)
        {
            case GAME_STATE_LEVEL:
                LevelGameStateUpdate(&game);
                break;
            case GAME_STATE_TRANSITION:
                TransitionGameStateUpdate(&game);
                break;
            case GAME_STATE_INVENTORY:
                InventoryGameStateUpdate(&game);
                break;
        }

        AudioUpdate();

        SDL_RenderClear(game.Renderer);

        switch (game.State)
        {
            case GAME_STATE_TRANSITION:
                DrawMap(game.Renderer, game.World->Map, game.World->Actors);
                TransitionGameStateDraw(&game);
                break;
            case GAME_STATE_LEVEL:
                DrawMap(game.Renderer, game.World->Map, game.World->Actors);
                LevelGameStateDraw(&game);
                break;
            case GAME_STATE_INVENTORY:
                InventoryGameStateDraw(&game);
                break;

        }

        SDL_RenderPresent(game.Renderer);
    }

    GameShutdown(&game);
}

//  ---------------------------------------------------------------------------
void GameShutdown(struct Game* game)
{
    GfxShutdown();

    if (game->BasePath != NULL)
    {
        SDL_free(game->BasePath);
        game->BasePath = NULL;
    }

    if (game->World != NULL)
    {
        DestroyWorld(&game->World);
        game->World = NULL;
    }

    if (game->Renderer != NULL)
    {
        SDL_DestroyRenderer(game->Renderer);
        game->Renderer = NULL;
    }

    if (game->Window != NULL)
    {
        SDL_DestroyWindow(game->Window);
        game->Window = NULL;
    }

    AudioShutdown();

    IMG_Quit();
    SDL_Quit();
}

//  ---------------------------------------------------------------------------
void LoadTexture(struct Game* game, SDL_Texture **texture, const char* assetFilename)
{
    char *fullpath = CreateTexturePath(game->BasePath, assetFilename);
    *texture = IMG_LoadTexture(game->Renderer, fullpath);
    free(fullpath);
}