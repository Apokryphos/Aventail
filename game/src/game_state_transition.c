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

static enum GameState BeginGameState = GAME_STATE_NONE;
static enum GameState EndGameState = GAME_STATE_NONE;

static struct MapLink* TransitionLink = NULL;
static char* TransitionMapName = NULL;

static enum Direction TransitionDirection = DIRECTION_NONE;
static int TransitionPhase = 0;
static const float TransitionDuration = 0.66f;
static float TransitionTicks = 0;

//  ---------------------------------------------------------------------------
static void load_map(
    struct Game* game,
    const char* asset_filename,
    struct Map** map,
    struct ActorList* actors)
{
    assert(*map == NULL);
    assert(asset_filename != NULL);

    char *fullpath = create_map_file_path(game->base_path, asset_filename);
    printf("%s\n", fullpath);
    FILE *file = fopen(fullpath, "rb");
    assert(file != NULL);
    free(fullpath);
    *map = load_map_from_file(file);
    load_actors_from_file(file, *map, actors);
    fclose(file);
}

//  ---------------------------------------------------------------------------
void UnloadMap(struct World* world)
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
void LoadMapLink(struct Game* game)
{
    assert(TransitionLink != NULL);
    assert(TransitionLink->dest_map != NULL);

    //  Copy destination link data before destroy_map frees it
    char* destMap = strdup(TransitionLink->dest_map);
    int destX = TransitionLink->dest_x;
    int destY = TransitionLink->dest_y;

    TransitionLink = NULL;

    struct World* world = game->world;

    UnloadMap(world);

    load_map(game, destMap, &world->map, world->actors);

    free(destMap);

    struct Tile* destTile = get_map_tile(world->map, destX, destY);
    assert(destTile != NULL);
    assert(in_map_bounds(world->map, destX, destY));

    world->player.actor->map = world->map;
    world->player.actor->tile = destTile;

    play_sfx(SFX_STEPS_ENTER);
}

//  ---------------------------------------------------------------------------
void BeginGameStateTransition(struct Game* game, enum GameState state)
{
    BeginGameState = game->state;
    EndGameState = state;

    game->state = GAME_STATE_TRANSITION;

    TransitionLink = NULL;
    TransitionDirection = DIRECTION_NONE;
    TransitionTicks = 0;
    TransitionPhase = 0;
}

//  ---------------------------------------------------------------------------
void BeginMapLinkTransition(
    struct Game* game,
    struct MapLink* link,
    enum Direction direction)
{
    BeginGameState = game->state;
    EndGameState = GAME_STATE_LOAD_MAP;

    game->state = GAME_STATE_TRANSITION;

    TransitionLink = link;
    TransitionDirection = direction;
    TransitionTicks = 0;
    TransitionPhase = 0;

    play_sfx(SFX_STEPS_EXIT);
}

//  ---------------------------------------------------------------------------
void BeginMapLoadTransition(struct Game* game, const char* mapName)
{
    BeginGameState = game->state;
    EndGameState = GAME_STATE_LOAD_MAP;

    game->state = GAME_STATE_TRANSITION;

    TransitionMapName = strdup(mapName);

    TransitionLink = NULL;
    TransitionDirection = DIRECTION_NONE;
    TransitionTicks = 0;
    TransitionPhase = 0;
}

//  ---------------------------------------------------------------------------
static void DrawTransitionEffect(struct Game* game)
{
    Uint8 alpha = 255;
    float progress = (TransitionTicks / TransitionDuration);

    SDL_Rect destRect;
    SDL_RenderGetViewport(game->renderer, &destRect);

    switch (TransitionDirection)
    {
        case DIRECTION_DOWN:
            destRect.y = TransitionPhase == 0 ?
            -destRect.h + (int)(destRect.h * progress) :
            (int)(destRect.h * progress);
            break;
        case DIRECTION_UP:
            destRect.y = TransitionPhase == 0 ?
            destRect.h - (int)(destRect.h * progress) :
            (int)(-destRect.h * progress);
            break;
        case DIRECTION_RIGHT:
            destRect.x = TransitionPhase == 0 ?
            -destRect.w + (int)(destRect.w * progress) :
            (int)(destRect.w * progress);
            break;
        case DIRECTION_LEFT:
            destRect.x = TransitionPhase == 0 ?
            destRect.w - (int)(destRect.w * progress) :
            (int)(-destRect.w * progress);
            break;
        case DIRECTION_NONE:
            alpha = TransitionPhase == 0 ?
            (int)(255 * progress) :
            255 - (int)(255 * progress);
            break;
    }

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(game->renderer, &destRect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void TransitionGameStateDraw(struct Game* game)
{
    if (TransitionPhase == 0)
    {
        draw_active_game_state(game, BeginGameState, 1);
    }
    else
    {
        draw_active_game_state(game, EndGameState, 1);
    }

    DrawTransitionEffect(game);
}

//  ---------------------------------------------------------------------------
void TransitionGameStateUpdate(struct Game* game)
{
    assert(game != NULL);

    TransitionTicks += game->elapsed_seconds;
    if (TransitionTicks >= TransitionDuration)
    {
        TransitionTicks -= TransitionDuration;
        ++TransitionPhase;

        if (TransitionPhase == 1)
        {
            if (EndGameState == GAME_STATE_LOAD_MAP)
            {
                if (TransitionLink != NULL)
                {
                    LoadMapLink(game);
                }
                else
                {
                    if (game->world->map != NULL)
                    {
                        UnloadMap(game->world);
                    }

                    load_map(game, TransitionMapName, &game->world->map, game->world->actors);
                    free(TransitionMapName);
                    TransitionMapName = NULL;

                    game->world->player.actor->map = game->world->map;
                    game->world->player.actor->tile = get_map_tile(game->world->map, 12, 10);

                }

                EndGameState = GAME_STATE_LEVEL;
            }
        }
        else
        {
            game->state = EndGameState;
        }
    }
}