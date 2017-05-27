#include "game.h"
#include "game_state.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "panel.h"
#include "render.h"
#include "status_widget.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

struct GuiScreen* StatusGuiScreen = NULL;
struct StatusWidget* StatusWidget = NULL;

//  ---------------------------------------------------------------------------
static void ExitStatusGameState(
    struct Game* game,
    enum GameState gameState)
{
    game->State = gameState;
    DeactivateGui();
    StatusGuiScreen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void ProcessStatusGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Cancel || inputDevice->Status)
    {
        ExitStatusGameState(game, GAME_STATE_LEVEL);
    }
    else if (inputDevice->Inventory)
    {
        ExitStatusGameState(game, GAME_STATE_INVENTORY);
    }
    else if (inputDevice->Gear)
    {
        ExitStatusGameState(game, GAME_STATE_GEAR);
    }
}

//  ---------------------------------------------------------------------------
static void InitStatusGuiScreen(SDL_Renderer* renderer)
{
    StatusGuiScreen = CreateGuiScreen();

    StatusWidget = CreateStatusWidget(StatusGuiScreen);

    //  Center status widget
    SDL_Rect viewport;
    SDL_RenderGetViewport(renderer, &viewport);
    SetStatusWidgetPosition(
        StatusWidget,
        (viewport.w / 2) - (StatusWidget->Panel->Width / 2),
        (viewport.h / 2) - (StatusWidget->Panel->Height / 2));

    AddGuiScreen(StatusGuiScreen);
}

//  ---------------------------------------------------------------------------
void StatusGameStateDraw(struct Game* game, int inTransition)
{
    DrawMap(game->Renderer, game->World->Map, game->World->Actors);
    GuiDraw(game);
}

//  ---------------------------------------------------------------------------
void StatusGameStateUpdate(struct Game* game)
{
    if (StatusGuiScreen == NULL)
    {
        InitStatusGuiScreen(game->Renderer);
    }

    StatusGuiScreen->Enabled = 1;

    ActivateGui();
    EnableCursor(0);

    ProcessStatusGameStateInput(game);

    UpdateStatusWidget(StatusWidget, game->World->Player.Actor);
}