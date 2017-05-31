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
static void exit_inventory_game_state(
    struct Game* game,
    enum GameState game_state)
{
    selected_item_type = ITEM_TYPE_NONE;
    inventory_widget->selected_item_index = 0;
    game->state = game_state;
    deactivate_gui();
    inventory_gui_screen->enabled = 0;
}

//  ---------------------------------------------------------------------------
static void select_next_item_type()
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
static void select_previous_item_type()
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
        exit_inventory_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->move_direction == DIRECTION_RIGHT)
    {
        select_next_item_type();
    }
    else if (input_device->move_direction == DIRECTION_LEFT)
    {
        select_previous_item_type();
    }
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
static void process_inventory_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->inventory)
    {
        exit_inventory_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->gear)
    {
        exit_inventory_game_state(game, GAME_STATE_GEAR);
    }
    else if (input_device->status)
    {
        exit_inventory_game_state(game, GAME_STATE_STATUS);
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
    draw_map(game->world->map, game->world->actors);
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
static int get_item_type_icon_tileset(enum ItemType item_type)
{
    switch (item_type)
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
static struct Panel* create_item_type_panel(enum ItemType item_type)
{
    struct Panel* panel = create_panel(
        get_item_type_category_string(item_type),
        PANEL_BORDER_STYLE_1);

    panel->width = 24;
    panel->height = 24;
    panel->background = 1;
    panel->icon.style = PANEL_ICON_STYLE_NORMAL;
    panel->icon.tileset_id = get_item_type_icon_tileset(item_type);

    return panel;
}

//  ---------------------------------------------------------------------------
static void init_inventory_gui_screen()
{
    inventory_gui_screen = create_gui_screen();

    for (int t = 0; t < ITEM_TYPE_COUNT; ++t)
    {
        struct Panel* panel = create_item_type_panel((enum ItemType)t);
        panel->X = 42 + (t * panel->width);
        panel->Y = 32;
        item_type_panels[t] = panel;
        add_panel_to_gui_screen(inventory_gui_screen, panel);
    }

    inventory_widget = create_inventory_widget(inventory_gui_screen);
    inventory_widget->panel->show_title = 0;

    //SDL_Rect viewport;
    //SDL_RenderGetViewport(renderer, &viewport);
    set_inventory_widget_position(inventory_widget, 16, 64);

    add_gui_screen(inventory_gui_screen);
}

//  ---------------------------------------------------------------------------
static struct Panel* get_selected_item_type_panel()
{
    return item_type_panels[(int)selected_item_type];
}

//  ---------------------------------------------------------------------------
static void update_cursor()
{
    enable_gui_cursor(1);

    if (inventory_gui_state == GUI_STATE_SELECT_ITEM_TYPE)
    {
        struct Panel* selected_panel = get_selected_item_type_panel();

        set_gui_cursor_position(
            selected_panel->X + 4,
            selected_panel->Y + 4);
    }
    else if (inventory_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(inventory_widget->selected_item_index < MAX_INVENTORY_ITEMS);

        assert(inventory_widget->selected_item_slot_widget != NULL);
        assert(inventory_widget->selected_item_slot_widget->item_icon_panel != NULL);
        set_gui_cursor_position(
            inventory_widget->selected_item_slot_widget->item_icon_panel->X + 4,
            inventory_widget->selected_item_slot_widget->item_icon_panel->Y + 4);
    }
}

//  ---------------------------------------------------------------------------
void update_inventory_game_state(struct Game* game)
{
    if (inventory_gui_screen == NULL)
    {
        init_inventory_gui_screen();
    }

    inventory_gui_screen->enabled = 1;

    if (inventory_widget->selected_item_index > inventory_widget->item_count -1)
    {
        inventory_widget->selected_item_index = inventory_widget->item_count -1;
    }
    if (inventory_widget->selected_item_index < 0)
    {
        inventory_widget->selected_item_index = 0;
    }

    activate_gui();
    process_inventory_game_state_input(game);

    struct Actor* actor = game->world->player.actor;

    inventory_widget->item_type = selected_item_type;
    update_inventory_widget(inventory_widget, actor->inventory);

    struct Panel* selected_panel = get_selected_item_type_panel();
    for (int p = 0; p < ITEM_TYPE_COUNT; ++p)
    {
        struct Panel* panel = item_type_panels[p];
        panel->show_title = panel == selected_panel;
    }

    update_cursor();
}