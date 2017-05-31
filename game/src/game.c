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

static struct Game* game = NULL;
static SDL_Window* window = NULL;

static void game_shutdown();

//  ---------------------------------------------------------------------------
static void game_init(int width, int height)
{
    assert(game == NULL);
    assert(width > 0);
    assert(height > 0);

    game = malloc(sizeof(struct Game));
    if (game == NULL)
    {
        perror("");
        fprintf(stderr, "Failed to create game.\n");
        exit(EXIT_FAILURE);
    }
    game->quit = 0;
    game->elapsed_seconds = 0;
    game->state = GAME_STATE_NONE;
    game->input_device = NULL;
    game->world = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "SDL failed to initialize: %sn", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    paths_init();

    if (audio_init() != 0)
    {
  		fprintf(stderr, "SDL2_mixer failed to initialize: %sn", Mix_GetError());
		exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow(
        "Aventail",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
		fprintf(stderr, "SDL failed to create window: %sn", SDL_GetError());
		exit(EXIT_FAILURE);
    }

    if (render_init(window) != 0)
    {
        printf("Failed to initialize graphics.\n");
        exit(EXIT_FAILURE);
    }

    game->elapsed_seconds = 0;
    game->quit = 0;
    game->state = GAME_STATE_LEVEL;

    IMG_Init(IMG_INIT_PNG);
}

//  ---------------------------------------------------------------------------
int game_main()
{
    if (atexit(&game_shutdown))
    {
        fprintf(stderr, "Failed to register atexit function.\n");
        return 1;
    }

    //  Use same seed everytime for now
    srand(0);

    game_init(800, 800);

    game->world = create_world();

    create_player_actor(game->world);

    begin_map_load_transition(game, "map01");

    struct InputDevice input_device = {0};
    game->input_device = &input_device;

    unsigned int last_ticks = 0;
    unsigned int ticks = 0;
    while (game->quit == 0)
    {
        last_ticks = ticks;
        ticks = SDL_GetTicks();
        game->elapsed_seconds = (ticks - last_ticks) / 1000.0f;

        update_input(game->input_device);
        game->quit = game->input_device->quit;

        update_active_game_state(game);

        update_gui(game);

        update_audio();

        render_clear();
        draw_game_state(game, game->state, 0);
        render_swap();
    }

    game_shutdown(&game);

    return 0;
}

//  ---------------------------------------------------------------------------
void game_shutdown()
{
    render_shutdown();

    if (game != NULL)
    {
        if (game->world != NULL)
        {
            destroy_world(&game->world);
            game->world = NULL;
        }

        // if (game->input_device != NULL)
        // {
        //     free(game->input_device);
        //     game->input_device = NULL;
        // }

        free(game);
    }

    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    shutdown_audio();

    IMG_Quit();
    SDL_Quit();
}