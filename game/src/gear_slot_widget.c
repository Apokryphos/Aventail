#include "gear_slot_widget.h"
#include "item_slot_widget.h"
#include "panel.h"
#include "gui_screen.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct GearSlotWidget* create_gear_slot_widget(
    struct GuiScreen* gui_screen,
    const char* slot_name,
    int slot_index)
{
    struct Panel* slot_panel = create_panel(slot_name, PANEL_BORDER_STYLE_2);
    slot_panel->width = 164;
    slot_panel->height = 56;
    slot_panel->x = 0;
    slot_panel->y = 0;
    slot_panel->background = 1;
    add_panel_to_gui_screen(gui_screen, slot_panel);

    struct GearSlotWidget* gear_slot_widget = malloc(sizeof(struct GearSlotWidget));
    gear_slot_widget->index = slot_index;
    gear_slot_widget->slot_panel = slot_panel;
    gear_slot_widget->item_slot_widget = create_item_slot_widget(gui_screen);

    set_item_slot_widget_position(
        gear_slot_widget->item_slot_widget,
        slot_panel->x,
        slot_panel->y);

    return gear_slot_widget;
}

//  ---------------------------------------------------------------------------
void set_gear_slot_widget_position(
    struct GearSlotWidget* widget,
    const int x,
    const int y)
{
    widget->slot_panel->x = x;
    widget->slot_panel->y = y;

    set_item_slot_widget_position(
        widget->item_slot_widget,
        widget->slot_panel->x,
        widget->slot_panel->y);
}

//  ---------------------------------------------------------------------------
void update_gear_slot_widget(
    struct GearSlotWidget* gear_slot_widget,
    struct Item* item)
{
    update_item_slot_widget(gear_slot_widget->item_slot_widget, item);
}