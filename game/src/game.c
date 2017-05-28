#include "game.h"
#include "actor.h"
#include "audio.h"
#include "game_state.h"
#include "game_state_transition.h"
#include "gui.h"
#include "input.h"
#include "input_device.h"
#include "map.h"
#include "map_link.h"
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
        "Aventail",
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

    CreatePlayerActor(game.World);

    BeginMapLoadTransition(&game, "map01");

    struct InputDevice inputDevice = {0};
    game.InputDevice = &inputDevice;

    unsigned int lastTicks = 0;
    unsigned int ticks = 0;
    while (game.Quit == 0)
    {
        lastTicks = ticks;
        ticks = SDL_GetTicks();
        game.ElapsedSeconds = (ticks - lastTicks) / 1000.0f;

        UpdateInput(game.InputDevice);
        game.Quit = game.InputDevice->Quit;

        UpdateGameState(&game);

        GuiUpdate(&game);

        AudioUpdate();

        SDL_RenderClear(game.Renderer);
        DrawGameState(&game, game.State, 0);
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
    char *fullpath = create_texture_file_path(game->BasePath, assetFilename);
    *texture = IMG_LoadTexture(game->Renderer, fullpath);
    free(fullpath);
}