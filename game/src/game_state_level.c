#include "game.h"
#include "actor.h"
#include "actor_list.h"
#include "gui.h"
#include "inventory.h"
#include "input_device.h"
#include "item.h"
#include "map.h"
#include "tile.h"
#include "render.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>

//  Mouse cursor position. Used by CursorOverActor.
static int CursorX = 0;
static int CursorY = 0;

static float HoverTicks = 0;
static const float HoverDuration = 0.33f;
static SDL_Rect HoverActorRect;

static int HoverActorHealth = 0;
static int HoverActorMaxHealth = 0;
static int HoverActorCash = 0;
static enum ActorType HoverActorType = ACTOR_TYPE_NONE;

static char* HoverActorCashString = NULL;
static char* HoverActorHealthString = NULL;
static char* HoverActorNameString = NULL;

//  ---------------------------------------------------------------------------
void ProcessLevelGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    game->World->Player.Actor->move_direction = inputDevice->MoveDirection;
    inputDevice->MoveDirection = DIRECTION_NONE;

    if (inputDevice->Gear)
    {
        game->State = GAME_STATE_GEAR;
    }
    else if (inputDevice->Inventory)
    {
        game->State = GAME_STATE_INVENTORY;
    }
    else if (inputDevice->Status)
    {
        game->State = GAME_STATE_STATUS;
    }

    if (inputDevice->DebugKillPlayerActor)
    {
        game->World->Player.Actor->health = 0;
    }
}

//  ---------------------------------------------------------------------------
int CursorOverActor(struct Actor* actor)
{
    assert(actor != NULL);

    if ((actor->type == ACTOR_TYPE_PLAYER ||
         actor->type == ACTOR_TYPE_VILLAIN) &&
        actor->tile != NULL &&
        actor->health > 0)
    {
        SDL_Rect rect;
        GetTileRect(actor->map, actor->tile, &rect, 1);

        return
            CursorX >= rect.x &&
            CursorX <= rect.x + rect.w &&
            CursorY >= rect.y &&
            CursorY <= rect.y +rect.h;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void LevelGameStateDraw(struct Game* game, int inTransition)
{
    if (game->World->Map != NULL)
    {
        DrawMap(game->Renderer, game->World->Map, game->World->Actors);
    }

    GuiDraw(game);

    if (inTransition)
    {
        return;
    }

    CursorX = game->InputDevice->CursorX;
    CursorY = game->InputDevice->CursorY;

    struct Actor* hoverActor = FindActor(game->World->Actors, &CursorOverActor);

    if (hoverActor != NULL)
    {
        HoverTicks += game->ElapsedSeconds;
        if (HoverTicks > HoverDuration)
        {
            HoverTicks = HoverDuration;
        }

        HoverActorType = hoverActor->type;

        if (hoverActor->cash != HoverActorCash)
        {
            HoverActorCash = hoverActor->cash;

            if (HoverActorCashString != NULL)
            {
                free(HoverActorCashString);
                HoverActorCashString = NULL;
            }
        }

        if (hoverActor->health != HoverActorHealth ||
            hoverActor->max_health != HoverActorMaxHealth)
        {
            HoverActorHealth = hoverActor->health;
            HoverActorMaxHealth = hoverActor->max_health;

            if (HoverActorHealthString != NULL)
            {
                free(HoverActorHealthString);
                HoverActorHealthString = NULL;
            }
        }

        if (HoverActorCashString == NULL)
        {
            asprintf(&HoverActorCashString, "Cash: %d", HoverActorCash);
        }

        if (HoverActorHealthString == NULL)
        {
            asprintf(&HoverActorHealthString, "Health: %d / %d", HoverActorHealth, HoverActorMaxHealth);
        }

        if (HoverActorNameString != NULL &&
            strcmp(hoverActor->name, HoverActorNameString) != 0)
        {
            free(HoverActorNameString);
            HoverActorNameString = NULL;
        }

        if (HoverActorNameString == NULL)
        {
            HoverActorNameString = strdup(hoverActor->name);
        }

        GetTileRect(hoverActor->map, hoverActor->tile, &HoverActorRect, 0);
    }
    else
    {
        HoverTicks -= game->ElapsedSeconds;
        if (HoverTicks < 0)
        {
            HoverTicks = 0;
        }
    }

    if (HoverActorHealthString != NULL)
    {
        float progress = HoverTicks / HoverDuration;

        int alpha = (int)(255 * progress);

        int textWidth, textHeight;
        MeasureText(HoverActorHealthString, &textWidth, &textHeight);

        const int lineHeight = textHeight + 2;
        const int padding = 8;

        SDL_Rect destRect = HoverActorRect;
        destRect.w = textWidth + padding;
        destRect.h = lineHeight * 2;
        destRect.y -= HoverActorRect.h + lineHeight;

        if (HoverActorType == ACTOR_TYPE_PLAYER)
        {
            destRect.h += lineHeight;
            destRect.y -= lineHeight;
        }

        if (alpha != 255)
        {
            SDL_SetRenderDrawBlendMode(game->Renderer, SDL_BLENDMODE_BLEND);
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(game->Renderer, &destRect);
        SDL_SetRenderDrawColor(game->Renderer, 255, 255, 255, alpha);
        SDL_RenderDrawRect(game->Renderer, &destRect);

        DrawTextAlpha(
            game->Renderer,
            HoverActorNameString,
            destRect.x + padding / 2,
            destRect.y + padding / 2,
            alpha);

        DrawTextAlpha(
            game->Renderer,
            HoverActorHealthString,
            destRect.x + padding / 2,
            destRect.y + lineHeight + padding / 2,
            alpha);

        if (HoverActorType == ACTOR_TYPE_PLAYER)
        {
            DrawTextAlpha(
                game->Renderer,
                HoverActorCashString,
                destRect.x + padding / 2,
                destRect.y + lineHeight + lineHeight + padding / 2,
                alpha);
        }

        SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 255);

        if (alpha != 255)
        {
            SDL_SetRenderDrawBlendMode(game->Renderer, SDL_BLENDMODE_NONE);
        }
    }
}

//  ---------------------------------------------------------------------------
void LevelGameStateUpdate(struct Game* game)
{
    ProcessLevelGameStateInput(game);
    SimulateWorld(game, game->World);
}