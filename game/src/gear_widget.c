#include "actor.h"
#include "audio.h"
#include "gear_widget.h"
#include "gear_slot_widget.h"
#include "input_device.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>

static const int GEAR_SLOT_COUNT = 4;

//  ---------------------------------------------------------------------------
struct GearWidget* create_gear_widget(struct GuiScreen* gui_screen)
{
    struct GearWidget* widget = malloc(sizeof(struct GearWidget));

    widget->selected_gear_slot = 0;

    widget->weapon_slot_widget = create_gear_slot_widget(gui_screen, "Weapon", 0);
    widget->armor_slot_widget = create_gear_slot_widget(gui_screen, "Armor", 1);
    widget->shield_slot_widget = create_gear_slot_widget(gui_screen, "Shield", 2);
    widget->accessory_slot_widget = create_gear_slot_widget(gui_screen, "Accessory", 3);

    set_gear_widget_position(widget, 0, 0);

    return widget;
}

//  ---------------------------------------------------------------------------
enum ItemType get_gear_widget_selected_item_type(struct GearWidget* widget)
{
    switch (widget->selected_gear_slot)
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
struct GearSlotWidget* get_gear_widget_selected_gear_slot(struct GearWidget* widget)
{
    assert(widget->selected_gear_slot < 4);

    switch (widget->selected_gear_slot)
    {
        default:
        case 0:
            return widget->weapon_slot_widget;
        case 1:
            return widget->armor_slot_widget;
        case 2:
            return widget->shield_slot_widget;
        case 3:
            return widget->accessory_slot_widget;
    }
}

//  ---------------------------------------------------------------------------
static void select_next_gear_slot(struct GearWidget* widget)
{
    play_sfx(SFX_MENU_NAV);

    ++widget->selected_gear_slot;
    if (widget->selected_gear_slot > GEAR_SLOT_COUNT - 1)
    {
        widget->selected_gear_slot = 0;
    }
}

//  ---------------------------------------------------------------------------
static void select_previous_gear_slot(struct GearWidget* widget)
{
    play_sfx(SFX_MENU_NAV);

    --widget->selected_gear_slot;
    if (widget->selected_gear_slot < 0)
    {
        widget->selected_gear_slot = GEAR_SLOT_COUNT - 1;
    }
}

//  ---------------------------------------------------------------------------
void process_gear_widget_input(
    struct GearWidget* widget,
    struct InputDevice* input_device)
{
    if (input_device->move_direction == DIRECTION_DOWN)
    {
        select_next_gear_slot(widget);
    }
    else if (input_device->move_direction == DIRECTION_UP)
    {
        select_previous_gear_slot(widget);
    }
}

//  ---------------------------------------------------------------------------
void reset_gear_widget(struct GearWidget* widget)
{
    widget->selected_gear_slot = 0;

}

//  ---------------------------------------------------------------------------
void set_gear_widget_position(
    struct GearWidget* widget,
    const int x,
    const int y)
{
    set_gear_slot_widget_position(
        widget->weapon_slot_widget,
        x,
        y + 32);

    set_gear_slot_widget_position(
        widget->armor_slot_widget,
        x,
        y + (widget->weapon_slot_widget->slot_panel->height + 32) + 32);

    set_gear_slot_widget_position(
        widget->shield_slot_widget,
        x,
        y + 2 * (widget->armor_slot_widget->slot_panel->height + 32) + 32);

    set_gear_slot_widget_position(
        widget->accessory_slot_widget,
        x,
        y + 3 * (widget->shield_slot_widget->slot_panel->height + 32) + 32);
}

//  ---------------------------------------------------------------------------
void update_gear_widget(struct GearWidget* widget, struct Actor* actor)
{
    update_gear_slot_widget(widget->armor_slot_widget, actor->gear.armor);
    update_gear_slot_widget(widget->shield_slot_widget, actor->gear.shield);
    update_gear_slot_widget(widget->weapon_slot_widget, actor->gear.weapon);
    update_gear_slot_widget(widget->accessory_slot_widget, actor->gear.accessory);
}