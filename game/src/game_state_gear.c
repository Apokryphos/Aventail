#include "actor.h"
#include "audio.h"
#include "game.h"
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

static const int GEAR_SLOT_COUNT = 4;

static int selected_gear_slot = 0;
static enum GuiState gear_gui_state = GUI_STATE_SELECT_GEAR_SLOT;

static struct GuiScreen* gear_gui_screen = NULL;
static struct GearSlotWidget weapon_slot_widget;
static struct GearSlotWidget armor_slot_widget;
static struct GearSlotWidget shield_slot_widget;
static struct GearSlotWidget accessory_slot_widget;
static struct InventoryWidget* inventory_widget = NULL;

//  ---------------------------------------------------------------------------
static enum ItemType get_selected_gear_slot_item_type()
{
    switch (selected_gear_slot)
    {
        default:
        case 0:
            return ITEM_TYPE_WEAPON;
        case 1:
            return ITEM_TYPE_ARMOR;
        case 2:
            return ITEM_TYPE_SHIELD;
        case 3:
            return ITEM_TYPE_ACCESSORY;
    }
}

//  ---------------------------------------------------------------------------
static void process_select_gear_slot_state_input(struct Game* game)
{
    assert(gear_gui_state == GUI_STATE_SELECT_GEAR_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->remove)
    {
        struct Actor* actor = game->world->player.actor;
        enum ItemType itemType = get_selected_gear_slot_item_type();
        remove_item_from_gear(actor, itemType);
        update_inventory_widget(inventory_widget, actor->inventory);
    }
    else if (input_device->accept)
    {
        if (inventory_widget->item_count > 0)
        {
            gear_gui_state = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            inventory_widget->selected_item_index = 0;
        }
    }
    else if (input_device->cancel)
    {
        enter_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->move_direction == DIRECTION_DOWN)
    {
        play_sfx(SFX_MENU_NAV);

        ++selected_gear_slot;
        if (selected_gear_slot > GEAR_SLOT_COUNT - 1)
        {
            selected_gear_slot = 0;
        }
    }
    else if (input_device->move_direction == DIRECTION_UP)
    {
        play_sfx(SFX_MENU_NAV);

        --selected_gear_slot;
        if (selected_gear_slot < 0)
        {
            selected_gear_slot = GEAR_SLOT_COUNT - 1;
        }
    }
}

//  ---------------------------------------------------------------------------
static void process_select_inventory_item_slot_state_input(struct Game* game)
{
    assert(gear_gui_state == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel)
    {
        inventory_widget->selected_item_index = 0;
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

            inventory_widget->selected_item_index = 0;
            gear_gui_state = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }

    if (input_device->move_direction == DIRECTION_DOWN)
    {
        select_next_inventory_widget_item_slot(inventory_widget);
    }

    if (input_device->move_direction == DIRECTION_UP)
    {
        select_previous_inventory_widget_item_slot(inventory_widget);
    }
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
static struct GearSlotWidget create_gear_slot_widget(
    struct GuiScreen* gui_screen,
    const char* slot_name,
    int slot_index)
{
    struct Panel* slot_panel = create_panel(slot_name, PANEL_BORDER_STYLE_2);
    slot_panel->width = 164;
    slot_panel->height = 56;
    slot_panel->x = 16;
    slot_panel->y = slot_index * (slot_panel->height + 32) + 32;
    slot_panel->background = 1;
    add_panel_to_gui_screen(gui_screen, slot_panel);

    struct GearSlotWidget gear_slot_widget =
    {
        .index = slot_index,
        .slot_panel = slot_panel,
        .item_slot_widget = create_item_slot_widget(gui_screen),
    };

    set_item_slot_widget_position(
        gear_slot_widget.item_slot_widget,
        slot_panel->x,
        slot_panel->y);

    return gear_slot_widget;
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

    weapon_slot_widget = create_gear_slot_widget(gear_gui_screen, "Weapon", 0);
    armor_slot_widget = create_gear_slot_widget(gear_gui_screen, "Armor", 1);
    shield_slot_widget = create_gear_slot_widget(gear_gui_screen, "Shield", 2);
    accessory_slot_widget = create_gear_slot_widget(gear_gui_screen, "Accessory", 3);

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
static struct GearSlotWidget* get_selected_gear_slot_gui()
{
    assert(selected_gear_slot < 4);

    switch (selected_gear_slot)
    {
        default:
        case 0:
            return &weapon_slot_widget;
        case 1:
            return &armor_slot_widget;
        case 2:
            return &shield_slot_widget;
        case 3:
            return &accessory_slot_widget;
    }
}

//  ---------------------------------------------------------------------------
static void update_cursor()
{
    if (gear_gui_state == GUI_STATE_SELECT_GEAR_SLOT)
    {
        assert(selected_gear_slot < 4);

        struct GearSlotWidget* selected_gear_slot = get_selected_gear_slot_gui();

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
    selected_gear_slot = 0;
    inventory_widget->selected_item_index = 0;

    gear_gui_screen->enabled = 0;
    enable_gui_cursor(0);
    deactivate_gui();
}

//  ---------------------------------------------------------------------------
void update_gear_game_state(struct Game* game)
{
    assert(selected_gear_slot >= 0 && selected_gear_slot < 4);
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
    update_gear_slot_widget(&armor_slot_widget, actor->gear.armor);
    update_gear_slot_widget(&shield_slot_widget, actor->gear.shield);
    update_gear_slot_widget(&weapon_slot_widget, actor->gear.weapon);
    update_gear_slot_widget(&accessory_slot_widget, actor->gear.accessory);

    inventory_widget->item_type = get_selected_gear_slot_item_type();
    update_inventory_widget(inventory_widget, actor->inventory);

    update_cursor();
}