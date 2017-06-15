#include "actor.h"
#include "audio.h"
#include "game.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "inventory.h"
#include "inventory_widget.h"
#include "item.h"
#include "item_slot_widget.h"
#include "item_type_widget.h"
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
static enum GuiState inventory_gui_state = GUI_STATE_SELECT_ITEM_TYPE;

static struct GuiScreen* inventory_gui_screen = NULL;
static struct ItemTypeWidget* item_type_widget = NULL;
static struct InventoryWidget* inventory_widget = NULL;

//  ---------------------------------------------------------------------------
static void process_select_item_type_state_input(struct Game* game)
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
        enter_game_state(game, GAME_STATE_LEVEL);
    }

    process_item_type_widget_input(item_type_widget, input_device);
}

//  ---------------------------------------------------------------------------
static void process_select_inventory_item_slot_state_input(struct Game* game)
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
            struct Actor* actor = game->world->player.actor;
            struct Item* item = inventory_widget->items[inventory_widget->selected_item_index];

            if (item->type == ITEM_TYPE_CONSUMABLE &&
                item->on_use != NULL)
            {
                if ((*item->on_use)(item, actor) == 0)
                {
                    inventory_widget->selected_item_index = 0;
                    inventory_gui_state = GUI_STATE_SELECT_ITEM_TYPE;
                }
                else
                {
                    play_sfx(SFX_ERROR);
                }
            }
            else
            {
                play_sfx(SFX_ERROR);
            }
        }
    }

    process_inventory_widget_input(inventory_widget, game->input_device);
}

//  ---------------------------------------------------------------------------
static void process_inventory_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->inventory)
    {
        enter_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->gear)
    {
        enter_game_state(game, GAME_STATE_GEAR);
    }
    else if (input_device->status)
    {
        enter_game_state(game, GAME_STATE_STATUS);
    }
    else if (input_device->help)
    {
        enter_game_state(game, GAME_STATE_HELP);
    }
    else
    {
        switch (inventory_gui_state)
        {
            case GUI_STATE_SELECT_ITEM_TYPE:
                process_select_item_type_state_input(game);
                break;
            case GUI_STATE_SELECT_INVENTORY_ITEM_SLOT:
                process_select_inventory_item_slot_state_input(game);
                break;
        }
    }
}

//  ---------------------------------------------------------------------------
void draw_inventory_game_state(struct Game* game, int in_transition)
{
    draw_map(game->world);
    draw_gui();
}

//  ---------------------------------------------------------------------------
static void init_inventory_gui_screen()
{
    inventory_gui_screen = create_gui_screen();

    item_type_widget = create_item_type_widget(inventory_gui_screen);
    set_item_type_widget_position(item_type_widget, 42, 32);

    inventory_widget = create_inventory_widget(inventory_gui_screen);
    inventory_widget->panel->show_title = 0;
    set_inventory_widget_position(inventory_widget, 16, 64);

    add_gui_screen(inventory_gui_screen);
}

//  ---------------------------------------------------------------------------
static void update_cursor()
{
    if (inventory_gui_state == GUI_STATE_SELECT_ITEM_TYPE)
    {
        struct Panel* selected_panel = get_selected_item_type_panel(item_type_widget);

        set_gui_cursor_position(
            selected_panel->x + 4,
            selected_panel->y + 4);
    }
    else if (inventory_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(inventory_widget->selected_item_index < MAX_INVENTORY_ITEMS);

        assert(inventory_widget->selected_item_slot_widget != NULL);
        assert(inventory_widget->selected_item_slot_widget->item_icon_panel != NULL);
        set_gui_cursor_position(
            inventory_widget->selected_item_slot_widget->item_icon_panel->x + 4,
            inventory_widget->selected_item_slot_widget->item_icon_panel->y + 4);
    }
}

//  ---------------------------------------------------------------------------
void activate_inventory_game_state(struct Game* game)
{
    if (inventory_gui_screen == NULL)
    {
        init_inventory_gui_screen();
    }

    activate_gui();
    enable_gui_cursor(1);
    inventory_gui_screen->enabled = 1;
}

//  ---------------------------------------------------------------------------
void deactivate_inventory_game_state(struct Game* game)
{
    reset_item_type_widget(item_type_widget);
    reset_inventory_widget(inventory_widget);

    deactivate_gui();
    enable_gui_cursor(0);
    inventory_gui_screen->enabled = 0;
}

//  ---------------------------------------------------------------------------
void update_inventory_game_state(struct Game* game)
{
    if (inventory_widget->selected_item_index > inventory_widget->item_count -1)
    {
        inventory_widget->selected_item_index = inventory_widget->item_count -1;
    }
    if (inventory_widget->selected_item_index < 0)
    {
        inventory_widget->selected_item_index = 0;
    }

    process_inventory_game_state_input(game);

    inventory_widget->item_type = item_type_widget->selected_item_type;
    struct Actor* actor = game->world->player.actor;
    update_inventory_widget(inventory_widget, actor->inventory);

    update_cursor();
}