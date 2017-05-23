#include "actor.h"
#include "actor_list.h"
#include "audio.h"
#include "direction.h"
#include "game.h"
#include "map.h"
#include "map_link.h"
#include "world.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

static struct MapLink* TransitionLink = NULL;
static enum Direction TransitionDirection = DIRECTION_NONE;
static int TransitionPhase = 0;
static const float TransitionDuration = 0.66f;
static float TransitionTicks = 0;

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

    //  Remove player from actors so it doesn't get freed
    RemoveActor(world->Actors, world->Player.Actor);

    DestroyActorList(&world->Actors);
    DestroyMap(&world->Map);

    game->World->Actors = CreateActorList();

    //  Add player actor back
    AddActor(world->Actors, world->Player.Actor);

    LoadMap(game, destMap, &world->Map, world->Actors);

    free(destMap);

    struct Tile* destTile = GetTile(world->Map, destX, destY);
    assert(destTile != NULL);
    assert(InBounds(world->Map, destX, destY));

    world->Player.Actor->Map = world->Map;
    world->Player.Actor->Tile = destTile;

    PlaySfx(SFX_STEPS_ENTER);
}

//  ---------------------------------------------------------------------------
void BeginTransition(
    struct Game* game, 
    struct MapLink* link, 
    enum Direction direction)
{
    game->State = GAME_STATE_TRANSITION;
    TransitionLink = link;
    TransitionDirection = direction;
    TransitionTicks = 0;
    TransitionPhase = 0;

    PlaySfx(SFX_STEPS_EXIT);
}

//  ---------------------------------------------------------------------------
void TransitionDraw(struct Game* game)
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
void TransitionUpdate(struct Game* game)
{
    assert(game != NULL);

    TransitionTicks += game->ElapsedSeconds;
    if (TransitionTicks >= TransitionDuration)
    {
        TransitionTicks -= TransitionDuration;
        ++TransitionPhase;

        if (TransitionPhase == 1)
        {
            LoadMapLink(game);
        }
        else
        {
            game->State = GAME_STATE_LEVEL;
        }
    }
}