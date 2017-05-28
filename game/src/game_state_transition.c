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
    RemoveActor(world->Actors, world->Player.Actor);

    DestroyActorList(&world->Actors);
    DestroyMap(&world->Map);

    world->Actors = CreateActorList();

    //  Add player actor back
    AddActorToBack(world->Actors, world->Player.Actor);
}

//  ---------------------------------------------------------------------------
void LoadMapLink(struct Game* game)
{
    assert(TransitionLink != NULL);
    assert(TransitionLink->DestMap != NULL);

    //  Copy destination link data before DestroyMap frees it
    char* destMap = strdup(TransitionLink->DestMap);
    int destX = TransitionLink->DestX;
    int destY = TransitionLink->DestY;

    TransitionLink = NULL;

    struct World* world = game->World;

    UnloadMap(world);

    LoadMap(game, destMap, &world->Map, world->Actors);

    free(destMap);

    struct Tile* destTile = GetTile(world->Map, destX, destY);
    assert(destTile != NULL);
    assert(InBounds(world->Map, destX, destY));

    world->Player.Actor->map = world->Map;
    world->Player.Actor->tile = destTile;

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

                    game->World->Player.Actor->map = game->World->Map;
                    game->World->Player.Actor->tile = GetTile(game->World->Map, 12, 10);

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