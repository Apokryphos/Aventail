#include "actor.h"
#include "audio.h"
#include "game.h"
#include "gear_widget.h"
#include "gear_slot_widget.h"
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
    GUI_STATE_SELECT_GEAR_SLOT,
    GUI_STATE_SELECT_INVENTORY_ITEM_SLOT
};

static enum GuiState gear_gui_state = GUI_STATE_SELECT_GEAR_SLOT;

static struct GuiScreen* gear_gui_screen = NULL;
static struct GearWidget* gear_widget;
static struct InventoryWidget* inventory_widget = NULL;

//  ---------------------------------------------------------------------------
static void process_select_gear_slot_state_input(struct Game* game)
{
    assert(gear_gui_state == GUI_STATE_SELECT_GEAR_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->remove)
    {
        struct Actor* actor = game->world->player.actor;
        enum ItemType itemType = get_gear_widget_selected_item_type(gear_widget);
        remove_item_from_gear(actor, itemType);
        update_inventory_widget(inventory_widget, actor->inventory);
    }
    else if (input_device->accept)
    {
        if (inventory_widget->item_count > 0)
        {
            gear_gui_state = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            reset_inventory_widget(inventory_widget);
        }
    }
    else if (input_device->cancel)
    {
        enter_game_state(game, GAME_STATE_LEVEL);
    }

    process_gear_widget_input(gear_widget, game->input_device);
}

//  ---------------------------------------------------------------------------
static void process_select_inventory_item_slot_state_input(struct Game* game)
{
    assert(gear_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel)
    {
        reset_inventory_widget(inventory_widget);
        gear_gui_state = GUI_STATE_SELECT_GEAR_SLOT;
    }

    if (input_device->accept)
    {
        if (inventory_widget->item_count > 0)
        {
            struct Actor* actor = game->world->player.actor;
            struct Item* item = inventory_widget->items[inventory_widget->selected_item_index];
            equip_item(actor, item);
            remove_item_from_inventory(actor->inventory, item);
            update_inventory_widget(inventory_widget, actor->inventory);

            reset_inventory_widget(inventory_widget);
            gear_gui_state = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }

    process_inventory_widget_input(inventory_widget, game->input_device);
}

//  ---------------------------------------------------------------------------
static void process_inventory_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->gear)
    {
        enter_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->inventory)
    {
        enter_game_state(game, GAME_STATE_INVENTORY);
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
        switch (gear_gui_state)
        {
            case GUI_STATE_SELECT_GEAR_SLOT:
                process_select_gear_slot_state_input(game);
                break;
            case GUI_STATE_SELECT_INVENTORY_ITEM_SLOT:
                process_select_inventory_item_slot_state_input(game);
                break;
        }
    }
}

//  ---------------------------------------------------------------------------
void draw_gear_game_state(struct Game* game, int in_transition)
{
    draw_map(game->world);
    draw_gui();
}

//  ---------------------------------------------------------------------------
static void init_gear_gui_screen()
{
    gear_gui_screen = create_gui_screen();

    gear_widget = create_gear_widget(gear_gui_screen);
    set_gear_widget_position(gear_widget, 16, 0);

    inventory_widget = create_inventory_widget(gear_gui_screen);

    SDL_Rect viewport;
    get_viewport(&viewport);
    set_inventory_widget_position(
        inventory_widget,
        viewport.w - inventory_widget->panel->width - 16,
        32);

    add_gui_screen(gear_gui_screen);
}

//  ---------------------------------------------------------------------------
static void update_cursor()
{
    if (gear_gui_state == GUI_STATE_SELECT_GEAR_SLOT)
    {
        assert(gear_widget->selected_gear_slot < 4);

        struct GearSlotWidget* selected_gear_slot = get_gear_widget_selected_gear_slot(gear_widget);

        set_gui_cursor_position(
            selected_gear_slot->item_slot_widget->item_icon_panel->x + 4,
            selected_gear_slot->item_slot_widget->item_icon_panel->y + 4);
    }
    else if (gear_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
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
void activate_gear_game_state(struct Game* game)
{
    if (gear_gui_screen == NULL)
    {
        init_gear_gui_screen();
    }

    gear_gui_screen->enabled = 1;
    enable_gui_cursor(1);
    activate_gui();
}

//  ---------------------------------------------------------------------------
void deactivate_gear_game_state(struct Game* game)
{
    reset_gear_widget(gear_widget);
    reset_inventory_widget(inventory_widget);

    gear_gui_screen->enabled = 0;
    enable_gui_cursor(0);
    deactivate_gui();
}

//  ---------------------------------------------------------------------------
void update_gear_game_state(struct Game* game)
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

    struct Actor* actor = game->world->player.actor;
    update_gear_widget(gear_widget, actor);

    inventory_widget->item_type = get_gear_widget_selected_item_type(gear_widget);
    update_inventory_widget(inventory_widget, actor->inventory);

    update_cursor();
}