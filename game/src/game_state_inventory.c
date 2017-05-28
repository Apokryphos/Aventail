#include "actor.h"
#include "game.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
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

/*
    The currently selected ItemType used to filter the InventoryWidget's display
*/
static enum ItemType selected_item_type = ITEM_TYPE_NONE;
static enum GuiState inventory_gui_state = GUI_STATE_SELECT_ITEM_TYPE;

static struct GuiScreen* inventory_gui_screen = NULL;
static struct Panel* item_type_panels[ITEM_TYPE_COUNT];
static struct InventoryWidget* inventory_widget = NULL;

//  ---------------------------------------------------------------------------
static void ExitInventoryGameState(
    struct Game* game,
    enum GameState gameState)
{
    selected_item_type = ITEM_TYPE_NONE;
    inventory_widget->selected_item_index = 0;
    game->state = gameState;
    DeactivateGui();
    inventory_gui_screen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void SelectNextItemType()
{
    int t = (int)selected_item_type;

    if (t < ITEM_TYPE_COUNT - 1)
    {
        ++t;
    }
    else
    {
        t = 0;
    }

    selected_item_type = (enum ItemType)t;
}

//  ---------------------------------------------------------------------------
static void SelectPreviousItemType()
{
    int t = (int)selected_item_type;

    if (t > 0)
    {
        --t;
    }
    else
    {
        t = ITEM_TYPE_COUNT - 1;
    }

    selected_item_type = (enum ItemType)t;
}

//  ---------------------------------------------------------------------------
static void ProcessSelectItemTypeStateInput(struct Game* game)
{
    assert(inventory_gui_state == GUI_STATE_SELECT_ITEM_TYPE);

    struct InputDevice* input_device = game->input_device;

    if (input_device->accept)
    {
        if (inventory_widget->item_count > 0)
        {
            inventory_gui_state = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            inventory_widget->selected_item_index = 0;
        }
    }
    else if (input_device->cancel)
    {
        ExitInventoryGameState(game, GAME_STATE_LEVEL);
    }
    else if (input_device->move_direction == DIRECTION_RIGHT)
    {
        SelectNextItemType();
    }
    else if (input_device->move_direction == DIRECTION_LEFT)
    {
        SelectPreviousItemType();
    }
}

//  ---------------------------------------------------------------------------
static void ProcessSelectInventoryItemSlotStateInput(struct Game* game)
{
    assert(inventory_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel)
    {
        inventory_widget->selected_item_index = 0;
        inventory_gui_state = GUI_STATE_SELECT_ITEM_TYPE;
    }
    else if (input_device->accept)
    {
        if (inventory_widget->item_count > 0)
        {
            // struct Actor* actor = game->world->player.actor;
            //struct Item* item = inventory_widget->Items[inventory_widget->selected_item_index];
            // equip_item(actor, item);
            // remove_item_from_inventory(actor->inventory, item);
            // update_inventory_widget(inventory_widget, actor->inventory);

            // inventory_widget->selected_item_index = 0;
            // inventory_gui_state = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }
    else if (input_device->move_direction == DIRECTION_DOWN)
    {
        select_next_inventory_widget_item_slot(inventory_widget);
    }
    else if (input_device->move_direction == DIRECTION_UP)
    {
        select_previous_inventory_widget_item_slot(inventory_widget);
    }
}

//  ---------------------------------------------------------------------------
static void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->inventory)
    {
        ExitInventoryGameState(game, GAME_STATE_LEVEL);
    }
    else if (input_device->gear)
    {
        ExitInventoryGameState(game, GAME_STATE_GEAR);
    }
    else if (input_device->status)
    {
        ExitInventoryGameState(game, GAME_STATE_STATUS);
    }
    else
    {
        switch (inventory_gui_state)
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
    DrawMap(game->renderer, game->world->map, game->world->actors);
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
    struct Panel* panel = CreatePanel(
        get_item_type_category_string(itemType),
        PANEL_BORDER_STYLE_1);

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
    inventory_gui_screen = CreateGuiScreen();

    for (int t = 0; t < ITEM_TYPE_COUNT; ++t)
    {
        struct Panel* panel = CreateItemTypePanel((enum ItemType)t);
        panel->X = 42 + (t * panel->Width);
        panel->Y = 32;
        item_type_panels[t] = panel;
        AddGuiScreenPanel(inventory_gui_screen, panel);
    }

    inventory_widget = create_inventory_widget(inventory_gui_screen);
    inventory_widget->panel->ShowTitle = 0;

    //SDL_Rect viewport;
    //SDL_RenderGetViewport(renderer, &viewport);
    set_inventory_widget_position(inventory_widget, 16, 64);

    AddGuiScreen(inventory_gui_screen);
}

//  ---------------------------------------------------------------------------
static struct Panel* GetSelectedItemTypePanel()
{
    return item_type_panels[(int)selected_item_type];
}

//  ---------------------------------------------------------------------------
static void UpdateCursor()
{
    EnableCursor(1);

    if (inventory_gui_state == GUI_STATE_SELECT_ITEM_TYPE)
    {
        struct Panel* selectedPanel = GetSelectedItemTypePanel();

        SetCursorPosition(
            selectedPanel->X + 4,
            selectedPanel->Y + 4);
    }
    else if (inventory_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(inventory_widget->selected_item_index < MAX_INVENTORY_ITEMS);

        assert(inventory_widget->selected_item_slot_widget != NULL);
        assert(inventory_widget->selected_item_slot_widget->ItemIconPanel != NULL);
        SetCursorPosition(
            inventory_widget->selected_item_slot_widget->ItemIconPanel->X + 4,
            inventory_widget->selected_item_slot_widget->ItemIconPanel->Y + 4);
    }
}

//  ---------------------------------------------------------------------------
void InventoryGameStateUpdate(struct Game* game)
{
    if (inventory_gui_screen == NULL)
    {
        InitInventoryGuiScreen(game->renderer);
    }

    inventory_gui_screen->Enabled = 1;

    if (inventory_widget->selected_item_index > inventory_widget->item_count -1)
    {
        inventory_widget->selected_item_index = inventory_widget->item_count -1;
    }
    if (inventory_widget->selected_item_index < 0)
    {
        inventory_widget->selected_item_index = 0;
    }

    ActivateGui();
    ProcessInventoryGameStateInput(game);

    struct Actor* actor = game->world->player.actor;

    inventory_widget->item_type = selected_item_type;
    update_inventory_widget(inventory_widget, actor->inventory);

    struct Panel* selectedPanel = GetSelectedItemTypePanel();
    for (int p = 0; p < ITEM_TYPE_COUNT; ++p)
    {
        struct Panel* panel = item_type_panels[p];
        panel->ShowTitle = panel == selectedPanel;
    }

    UpdateCursor();
}