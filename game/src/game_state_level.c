#include "game.h"
#include "actor.h"
#include "actor_list.h"
#include "input.h"
#include "tile.h"
#include "render.h"
#include "world.h"
#include <assert.h>
#include <SDL2/SDL.h>

static int ActorCursorX = 0;
static int ActorCursorY = 0;

//  ---------------------------------------------------------------------------
void ProcessLevelInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    game->World->Player.Actor->MoveDirection = inputDevice->MoveDirection;
    inputDevice->MoveDirection = DIRECTION_NONE;
}

//  ---------------------------------------------------------------------------
int CursorOverActor(struct Actor* actor)
{
    assert(actor != NULL);

    if (actor->Tile != NULL)
    {
        int tileWidth = 32;
        int tileHeight = 32;

        int x = actor->Tile->X * tileWidth;
        int y = actor->Tile->Y * tileHeight;

        return 
            ActorCursorX >= x && 
            ActorCursorX <= x + tileWidth &&
            ActorCursorY >= y && 
            ActorCursorY <= y + tileHeight;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void LevelDraw(struct Game* game)
{
    ActorCursorX = game->InputDevice->CursorX;
    ActorCursorY = game->InputDevice->CursorY;

    struct Actor* hoverActor = FindActor(game->World->Actors, &CursorOverActor);
    if (hoverActor != NULL)
    {
        SDL_Rect destRect = 
        {
            .x = hoverActor->Tile->X * 32,
            .y = hoverActor->Tile->Y * 32,
        };

        MeasureText(hoverActor->Name, &destRect.w, &destRect.h);
        destRect.w += 8;
        destRect.y -= destRect.h;

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(game->Renderer, &destRect);
        SDL_SetRenderDrawColor(game->Renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(game->Renderer, &destRect);

        DrawText(game->Renderer, hoverActor->Name, destRect.x + 4, destRect.y + 4);
        
        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 255);
    }
}

//  ---------------------------------------------------------------------------
void LevelUpdate(struct Game* game)
{
    ProcessLevelInput(game);
    SimulateWorld(game, game->World);
}