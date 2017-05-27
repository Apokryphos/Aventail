#include "actor.h"
#include "game.h"
#include "gui.h"
#include "gui_screen.h"
#include "input.h"
#include "inventory.h"
#include "inventory_widget.h"
#include "item.h"
#include "item_slot_widget.h"
#include "panel.h"
#include "player.h"
#include "render.h"
#include "world.h"
#include <assert.h>
#include <SDL2/SDL.h>

enum GuiState
{
    GUI_STATE_SELECT_ITEM_TYPE,
    GUI_STATE_SELECT_INVENTORY_ITEM_SLOT,
};

static enum ItemType SelectedItemType = ITEM_TYPE_NONE;

static struct GuiScreen* InventoryGuiScreen = NULL;
static struct Panel* ItemTypePanels[ITEM_TYPE_COUNT];
static struct InventoryWidget* InventoryWidget = NULL;

static enum GuiState InventoryGuiState = GUI_STATE_SELECT_ITEM_TYPE;

//  ---------------------------------------------------------------------------
static void ExitInventoryGameState(struct Game* game)
{
    SelectedItemType = ITEM_TYPE_NONE;
    InventoryWidget->SelectedSlotIndex = 0;
    game->State = GAME_STATE_LEVEL;
    DeactivateGui();
    InventoryGuiScreen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void SelectNextItemType()
{
    int t = (int)SelectedItemType;

    if (t < ITEM_TYPE_COUNT - 1)
    {
        ++t;
    }
    else
    {
        t = 0;
    }

    SelectedItemType = (enum ItemType)t;
}

//  ---------------------------------------------------------------------------
static void SelectPreviousItemType()
{
    int t = (int)SelectedItemType;

    if (t > 0)
    {
        --t;
    }
    else
    {
        t = ITEM_TYPE_COUNT - 1;
    }

    SelectedItemType = (enum ItemType)t;
}

//  ---------------------------------------------------------------------------
static void ProcessSelectItemTypeStateInput(struct Game* game)
{
    assert(InventoryGuiState == GUI_STATE_SELECT_ITEM_TYPE);

    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Accept)
    {
        if (InventoryWidget->ItemCount > 0)
        {
            InventoryGuiState = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            InventoryWidget->SelectedSlotIndex = 0;
        }
    }
    else if (inputDevice->Cancel)
    {
        ExitInventoryGameState(game);
    }
    else if (inputDevice->MoveDirection == DIRECTION_RIGHT)
    {
        SelectNextItemType();
    }
    else if (inputDevice->MoveDirection == DIRECTION_LEFT)
    {
        SelectPreviousItemType();
    }
}

//  ---------------------------------------------------------------------------
static void ProcessSelectInventoryItemSlotStateInput(struct Game* game)
{
    assert(InventoryGuiState == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT);
    
    struct InputDevice* inputDevice = game->InputDevice;
    
    if (inputDevice->Cancel)
    {
        InventoryWidget->SelectedSlotIndex = 0;
        InventoryGuiState = GUI_STATE_SELECT_ITEM_TYPE;
    } 
    else if (inputDevice->Accept)
    {
        if (InventoryWidget->ItemCount > 0)
        {
            // struct Actor* actor = game->World->Player.Actor;
            //struct Item* item = InventoryWidget->Items[InventoryWidget->SelectedSlotIndex];
            // EquipItem(actor, item);
            // RemoveInventoryItem(actor->Inventory, item);
            // UpdateInventoryWidget(InventoryWidget, actor->Inventory);

            // InventoryWidget->SelectedSlotIndex = 0;
            // InventoryGuiState = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }
    else if (inputDevice->MoveDirection == DIRECTION_DOWN)
    {
        SelectNextInventoryWidgetItemSlot(InventoryWidget);
    }
    else if (inputDevice->MoveDirection == DIRECTION_UP)
    {
        SelectPreviousInventoryWidgetItemSlot(InventoryWidget);
    }
}

//  ---------------------------------------------------------------------------
static void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Inventory)
    {
        ExitInventoryGameState(game);
    }
    else
    {
        switch (InventoryGuiState)
        {
            case GUI_STATE_SELECT_ITEM_TYPE:
                ProcessSelectItemTypeStateInput(game);
                break;
            case GUI_STATE_SELECT_INVENTORY_ITEM_SLOT:
                ProcessSelectInventoryItemSlotStateInput(game);
                break;
        }
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateDraw(struct Game* game, int inTransition)
{
    DrawMap(game->Renderer, game->World->Map, game->World->Actors);
    GuiDraw(game);
}

//  ---------------------------------------------------------------------------
static int GetItemTypeIconTilesetId(enum ItemType itemType)
{
    switch (itemType)
    {
        default:
            return 248;
        case ITEM_TYPE_NONE:
            return 91;
        case ITEM_TYPE_CONSUMABLE:
            return 98;
        case ITEM_TYPE_WEAPON:
            return 137;
        case ITEM_TYPE_ARMOR:
            return 166;
        case ITEM_TYPE_SHIELD:
            return 176;
        case ITEM_TYPE_ACCESSORY:
            return 92;
    }
}

//  ---------------------------------------------------------------------------
static struct Panel* CreateItemTypePanel(enum ItemType itemType)
{
    struct Panel* panel = CreatePanel(NULL, PANEL_BORDER_STYLE_1);

    panel->Width = 24;
    panel->Height = 24;
    panel->Background = 1;
    panel->Icon.Style = PANEL_ICON_STYLE_NORMAL;
    panel->Icon.TilesetId = GetItemTypeIconTilesetId(itemType);
 
    return panel;
}

//  ---------------------------------------------------------------------------
static void InitInventoryGuiScreen(SDL_Renderer* renderer)
{
    InventoryGuiScreen = CreateGuiScreen();

    for (int t = 0; t < ITEM_TYPE_COUNT; ++t)
    {
        struct Panel* panel = CreateItemTypePanel((enum ItemType)t);
        panel->X = 16 + (t * panel->Width);
        panel->Y = 16;
        ItemTypePanels[t] = panel;
        AddGuiScreenPanel(InventoryGuiScreen, panel);
    }

    InventoryWidget = CreateInventoryWidget(InventoryGuiScreen);

    //SDL_Rect viewport;
    //SDL_RenderGetViewport(renderer, &viewport);
    SetInventoryWidgetPosition(InventoryWidget, 16, 64);

    AddGuiScreen(InventoryGuiScreen);
}

//  ---------------------------------------------------------------------------
static struct Panel* GetSelectedItemTypePanel()
{
    return ItemTypePanels[(int)SelectedItemType];
}

//  ---------------------------------------------------------------------------
static void UpdateItemTypeCursor()
{
    if (InventoryGuiState == GUI_STATE_SELECT_ITEM_TYPE)
    {
        struct Panel* selectedPanel = GetSelectedItemTypePanel();

        SetCursorPosition(
            selectedPanel->X + 4,
            selectedPanel->Y + 4);
    }
    else if (InventoryGuiState == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(InventoryWidget->SelectedSlotIndex < MaxInventoryItems);

        assert(InventoryWidget->SelectedItemSlotWidget != NULL);
        assert(InventoryWidget->SelectedItemSlotWidget->ItemIconPanel != NULL);
        SetCursorPosition(
            InventoryWidget->SelectedItemSlotWidget->ItemIconPanel->X + 4,
            InventoryWidget->SelectedItemSlotWidget->ItemIconPanel->Y + 4);
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateUpdate(struct Game* game)
{
    if (InventoryGuiScreen == NULL)
    {
        InitInventoryGuiScreen(game->Renderer);
    }

    InventoryGuiScreen->Enabled = 1;

    if (InventoryWidget->SelectedSlotIndex > InventoryWidget->ItemCount -1)
    {
        InventoryWidget->SelectedSlotIndex = InventoryWidget->ItemCount -1;
    }
    if (InventoryWidget->SelectedSlotIndex < 0)
    {
        InventoryWidget->SelectedSlotIndex = 0;
    }

    ActivateGui();
    EnableCursor(1);
    ProcessInventoryGameStateInput(game);

    struct Actor* actor = game->World->Player.Actor;

    InventoryWidget->ItemType = SelectedItemType;
    UpdateInventoryWidget(InventoryWidget, actor->Inventory);

    UpdateItemTypeCursor();
}