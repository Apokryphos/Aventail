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
void UnloadMap(struct World* world)
{
    //  Remove player from actors so it doesn't get freed
    remove_actor_from_actor_list(world->Actors, world->Player.actor);

    destroy_actor_list(&world->Actors);
    destroy_map(&world->Map);

    world->Actors = create_actor_list();

    //  Add player actor back
    add_actor_to_actor_list_back(world->Actors, world->Player.actor);
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

    struct World* world = game->World;

    UnloadMap(world);

    LoadMap(game, destMap, &world->Map, world->Actors);

    free(destMap);

    struct Tile* destTile = get_map_tile(world->Map, destX, destY);
    assert(destTile != NULL);
    assert(in_map_bounds(world->Map, destX, destY));

    world->Player.actor->map = world->Map;
    world->Player.actor->tile = destTile;

    PlaySfx(SFX_STEPS_ENTER);
}

//  ---------------------------------------------------------------------------
void BeginGameStateTransition(struct Game* game, enum GameState state)
{
    BeginGameState = game->State;
    EndGameState = state;

    game->State = GAME_STATE_TRANSITION;

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
    BeginGameState = game->State;
    EndGameState = GAME_STATE_LOAD_MAP;

    game->State = GAME_STATE_TRANSITION;

    TransitionLink = link;
    TransitionDirection = direction;
    TransitionTicks = 0;
    TransitionPhase = 0;

    PlaySfx(SFX_STEPS_EXIT);
}

//  ---------------------------------------------------------------------------
void BeginMapLoadTransition(struct Game* game, const char* mapName)
{
    BeginGameState = game->State;
    EndGameState = GAME_STATE_LOAD_MAP;

    game->State = GAME_STATE_TRANSITION;

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
    SDL_RenderGetViewport(game->Renderer, &destRect);

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
        SDL_SetRenderDrawBlendMode(game->Renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(game->Renderer, &destRect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(game->Renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void TransitionGameStateDraw(struct Game* game)
{
    if (TransitionPhase == 0)
    {
        DrawGameState(game, BeginGameState, 1);
    }
    else
    {
        DrawGameState(game, EndGameState, 1);
    }

    DrawTransitionEffect(game);
}

//  ---------------------------------------------------------------------------
void TransitionGameStateUpdate(struct Game* game)
{
    assert(game != NULL);

    TransitionTicks += game->ElapsedSeconds;
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
                    if (game->World->Map != NULL)
                    {
                        UnloadMap(game->World);
                    }

                    LoadMap(game, TransitionMapName, &game->World->Map, game->World->Actors);
                    free(TransitionMapName);
                    TransitionMapName = NULL;

                    game->World->Player.actor->map = game->World->Map;
                    game->World->Player.actor->tile = get_map_tile(game->World->Map, 12, 10);

                }

                EndGameState = GAME_STATE_LEVEL;
            }
        }
        else
        {
            game->State = EndGameState;
        }
    }
}