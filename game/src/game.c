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
void init_game(struct Game* game, int width, int height)
{
    assert(game != NULL);
    assert(width > 0);
    assert(height > 0);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
		printf("SDL failed to initialize: %sn", SDL_GetError());
		exit(1);
	}

    game->base_path = SDL_GetBasePath();
    if (game->base_path == NULL)
    {
        game->base_path = SDL_strdup("./");
    }

    if (init_audio(game) != 0)
    {
  		printf("SDL2_mixer failed to initialize: %sn", Mix_GetError());
		exit(1);
    }

    game->window = SDL_CreateWindow(
        "Aventail",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (game->window == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

    if (game->renderer == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    //  Clear window
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    SDL_RenderPresent(game->renderer);

    game->elapsed_seconds = 0;
    game->quit = 0;
    game->state = GAME_STATE_LEVEL;

    IMG_Init(IMG_INIT_PNG);
}

//  ---------------------------------------------------------------------------
void game_main()
{
    //  Use same seed everytime for now
    srand(0);

    struct Game game = {0};

    init_game(&game, 800, 800);

    if (GfxInit(&game) != 0)
    {
        printf("Failed to initialize graphics.\n");
        shutdown_game(&game);
    }

    game.world = create_world();

    create_player_actor(game.world);

    BeginMapLoadTransition(&game, "map01");

    struct InputDevice input_device = {0};
    game.input_device = &input_device;

    unsigned int last_ticks = 0;
    unsigned int ticks = 0;
    while (game.quit == 0)
    {
        last_ticks = ticks;
        ticks = SDL_GetTicks();
        game.elapsed_seconds = (ticks - last_ticks) / 1000.0f;

        update_input(game.input_device);
        game.quit = game.input_device->Quit;

        update_active_game_state(&game);

        GuiUpdate(&game);

        update_audio();

        SDL_RenderClear(game.renderer);
        draw_active_game_state(&game, game.state, 0);
        SDL_RenderPresent(game.renderer);
    }

    shutdown_game(&game);
}

//  ---------------------------------------------------------------------------
void shutdown_game(struct Game* game)
{
    GfxShutdown();

    if (game->base_path != NULL)
    {
        SDL_free(game->base_path);
        game->base_path = NULL;
    }

    if (game->world != NULL)
    {
        destroy_world(&game->world);
        game->world = NULL;
    }

    if (game->renderer != NULL)
    {
        SDL_DestroyRenderer(game->renderer);
        game->renderer = NULL;
    }

    if (game->window != NULL)
    {
        SDL_DestroyWindow(game->window);
        game->window = NULL;
    }

    shutdown_audio();

    IMG_Quit();
    SDL_Quit();
}

//  ---------------------------------------------------------------------------
void load_texture(
    struct Game* game,
    SDL_Texture **texture,
    const char* asset_filename)
{
    char *full_path = create_texture_file_path(game->base_path, asset_filename);
    *texture = IMG_LoadTexture(game->renderer, full_path);
    free(full_path);
}