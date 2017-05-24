#include "actor.h"
#include "game.h"
#include "input.h"
#include "inventory.h"
#include "item.h"
#include "panel.h"
#include "player.h"
#include "render.h"
#include "world.h"

static struct Panel GearPanel;

//  ---------------------------------------------------------------------------
void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Cancel || inputDevice->Inventory)
    {
        game->State = GAME_STATE_LEVEL;
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateDraw(struct Game* game, int inTransition)
{
    struct Actor* actor = game->World->Player.Actor;

    const int padding = 8;
    const int itemSize = 16;

    GearPanel.X = 16;
    GearPanel.Y = 16;
    GearPanel.Width = 128;
    GearPanel.Height = MaxInventoryItems * (itemSize + padding) + (padding * 2);

    char* inventoryTitleText = "Inventory";
    int inventoryTitleWidth, inventoryTitleHeight;
    MeasureText(inventoryTitleText, &inventoryTitleWidth, &inventoryTitleHeight);
    int inventoryTitleX = GearPanel.X + (GearPanel.Width / 2) - (inventoryTitleWidth / 2);
    int inventoryTitleY = GearPanel.Y - inventoryTitleHeight;
    DrawText(game->Renderer, inventoryTitleText, inventoryTitleX, inventoryTitleY);

    DrawPanel(game->Renderer, &GearPanel);

    int itemX = GearPanel.X + padding;
    int itemY = GearPanel.Y + padding;
    for (int n = 0; n < MaxInventoryItems; ++n)
    {
        struct Item* item = actor->Inventory->Items[n];
        if (item != NULL)
        {
            DrawTilesetTile(game->Renderer, item->TilesetId, itemX, itemY);

            int itemTextWidth, itemTextHeight;
            MeasureText(item->Name, &itemTextWidth, &itemTextHeight);

            DrawText(
                game->Renderer,
                item->Name,
                itemX + itemSize + padding,
                itemY + 2 + itemSize / 2 - itemTextHeight / 2);

            itemY += itemSize + padding;
        }
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateUpdate(struct Game* game)
{
    ProcessInventoryGameStateInput(game);
}