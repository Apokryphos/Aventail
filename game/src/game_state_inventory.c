#include "game.h"
#include "input.h"
#include "panel.h"
#include "render.h"

struct Panel GearPanel;

//  ---------------------------------------------------------------------------
void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Cancel)
    {
        game->State = GAME_STATE_LEVEL;
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateDraw(struct Game* game)
{
    GearPanel.X = 32;
    GearPanel.Y = 32;
    GearPanel.Width = 128;
    GearPanel.Height = 128;

    DrawPanel(game->Renderer, &GearPanel);
}

//  ---------------------------------------------------------------------------
void InventoryGameStateUpdate(struct Game* game)
{
    ProcessInventoryGameStateInput(game);
}