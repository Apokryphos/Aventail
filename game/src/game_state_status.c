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
    game->state = gameState;
    deactivate_gui();
    StatusGuiScreen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void ProcessStatusGameStateInput(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel || input_device->status)
    {
        ExitStatusGameState(game, GAME_STATE_LEVEL);
    }
    else if (input_device->inventory)
    {
        ExitStatusGameState(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->gear)
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

    add_gui_screen(StatusGuiScreen);
}

//  ---------------------------------------------------------------------------
void StatusGameStateDraw(struct Game* game, int inTransition)
{
    DrawMap(game->renderer, game->world->map, game->world->actors);
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
void StatusGameStateUpdate(struct Game* game)
{
    if (StatusGuiScreen == NULL)
    {
        InitStatusGuiScreen(game->renderer);
    }

    StatusGuiScreen->Enabled = 1;

    activate_gui();
    enable_gui_cursor(0);

    ProcessStatusGameStateInput(game);

    UpdateStatusWidget(StatusWidget, game->world->player.actor);
}