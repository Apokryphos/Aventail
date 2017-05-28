#include "actor.h"
#include "actor_list.h"
#include "audio.h"
#include "direction.h"
#include "game.h"
#include "map.h"
#include "map_link.h"
#include "map_load_util.h"
#include "paths.h"
#include "world.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

static enum GameState begin_game_state = GAME_STATE_NONE;
static enum GameState end_game_state = GAME_STATE_NONE;

static struct MapLink* transition_link = NULL;
static char* transition_map_name = NULL;

static enum Direction transition_direction = DIRECTION_NONE;
static int transition_phase = 0;
static const float TRANSITION_DURATION = 0.66f;
static float transition_ticks = 0;

//  ---------------------------------------------------------------------------
static void load_map(
    struct Game* game,
    const char* asset_filename,
    struct Map** map,
    struct ActorList* actors)
{
    assert(*map == NULL);
    assert(asset_filename != NULL);

    char *full_path = create_map_file_path(game->base_path, asset_filename);
    printf("%s\n", full_path);
    FILE *file = fopen(full_path, "rb");
    assert(file != NULL);
    free(full_path);
    *map = load_map_from_file(file);
    load_actors_from_file(file, *map, actors);
    fclose(file);
}

//  ---------------------------------------------------------------------------
static void unload_map(struct World* world)
{
    //  Remove player from actors so it doesn't get freed
    remove_actor_from_actor_list(world->actors, world->player.actor);

    destroy_actor_list(&world->actors);
    destroy_map(&world->map);

    world->actors = create_actor_list();

    //  Add player actor back
    add_actor_to_actor_list_back(world->actors, world->player.actor);
}

//  ---------------------------------------------------------------------------
static void load_map_link(struct Game* game)
{
    assert(transition_link != NULL);
    assert(transition_link->dest_map != NULL);

    //  Copy destination link data before destroy_map frees it
    char* dest_map = strdup(transition_link->dest_map);
    int dest_x = transition_link->dest_x;
    int dest_y = transition_link->dest_y;

    transition_link = NULL;

    struct World* world = game->world;

    unload_map(world);

    load_map(game, dest_map, &world->map, world->actors);

    free(dest_map);

    struct Tile* dest_tile = get_map_tile(world->map, dest_x, dest_y);
    assert(dest_tile != NULL);
    assert(in_map_bounds(world->map, dest_x, dest_y));

    world->player.actor->map = world->map;
    world->player.actor->tile = dest_tile;

    play_sfx(SFX_STEPS_ENTER);
}

//  ---------------------------------------------------------------------------
void begin_game_state_transition(struct Game* game, enum GameState state)
{
    begin_game_state = game->state;
    end_game_state = state;

    game->state = GAME_STATE_TRANSITION;

    transition_link = NULL;
    transition_direction = DIRECTION_NONE;
    transition_ticks = 0;
    transition_phase = 0;
}

//  ---------------------------------------------------------------------------
void begin_map_link_transition(
    struct Game* game,
    struct MapLink* link,
    enum Direction direction)
{
    begin_game_state = game->state;
    end_game_state = GAME_STATE_LOAD_MAP;

    game->state = GAME_STATE_TRANSITION;

    transition_link = link;
    transition_direction = direction;
    transition_ticks = 0;
    transition_phase = 0;

    play_sfx(SFX_STEPS_EXIT);
}

//  ---------------------------------------------------------------------------
void begin_map_load_transition(struct Game* game, const char* map_name)
{
    begin_game_state = game->state;
    end_game_state = GAME_STATE_LOAD_MAP;

    game->state = GAME_STATE_TRANSITION;

    transition_map_name = strdup(map_name);

    transition_link = NULL;
    transition_direction = DIRECTION_NONE;
    transition_ticks = 0;
    transition_phase = 0;
}

//  ---------------------------------------------------------------------------
static void DrawTransitionEffect(struct Game* game)
{
    Uint8 alpha = 255;
    float progress = (transition_ticks / TRANSITION_DURATION);

    SDL_Rect dest_rect;
    SDL_RenderGetViewport(game->renderer, &dest_rect);

    switch (transition_direction)
    {
        case DIRECTION_DOWN:
            dest_rect.y = transition_phase == 0 ?
            -dest_rect.h + (int)(dest_rect.h * progress) :
            (int)(dest_rect.h * progress);
            break;
        case DIRECTION_UP:
            dest_rect.y = transition_phase == 0 ?
            dest_rect.h - (int)(dest_rect.h * progress) :
            (int)(-dest_rect.h * progress);
            break;
        case DIRECTION_RIGHT:
            dest_rect.x = transition_phase == 0 ?
            -dest_rect.w + (int)(dest_rect.w * progress) :
            (int)(dest_rect.w * progress);
            break;
        case DIRECTION_LEFT:
            dest_rect.x = transition_phase == 0 ?
            dest_rect.w - (int)(dest_rect.w * progress) :
            (int)(-dest_rect.w * progress);
            break;
        case DIRECTION_NONE:
            alpha = transition_phase == 0 ?
            (int)(255 * progress) :
            255 - (int)(255 * progress);
            break;
    }

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(game->renderer, &dest_rect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void draw_transition_game_state(struct Game* game)
{
    if (transition_phase == 0)
    {
        draw_active_game_state(game, begin_game_state, 1);
    }
    else
    {
        draw_active_game_state(game, end_game_state, 1);
    }

    DrawTransitionEffect(game);
}

//  ---------------------------------------------------------------------------
void update_transition_game_state(struct Game* game)
{
    assert(game != NULL);

    transition_ticks += game->elapsed_seconds;
    if (transition_ticks >= TRANSITION_DURATION)
    {
        transition_ticks -= TRANSITION_DURATION;
        ++transition_phase;

        if (transition_phase == 1)
        {
            if (end_game_state == GAME_STATE_LOAD_MAP)
            {
                if (transition_link != NULL)
                {
                    load_map_link(game);
                }
                else
                {
                    if (game->world->map != NULL)
                    {
                        unload_map(game->world);
                    }

                    load_map(
                        game,
                        transition_map_name,
                        &game->world->map,
                        game->world->actors);

                    free(transition_map_name);
                    transition_map_name = NULL;

                    game->world->player.actor->map = game->world->map;

                    game->world->player.actor->tile =
                        get_map_tile(game->world->map, 12, 10);

                }

                end_game_state = GAME_STATE_LEVEL;
            }
        }
        else
        {
            game->state = end_game_state;
        }
    }
}