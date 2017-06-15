#ifndef GEAR_WIDGET_HEADER
#define GEAR_WIDGET_HEADER

#include "item_type.h"

struct Actor;
struct GearSlotWidget;
struct GuiScreen;
struct InputDevice;

struct GearWidget
{
    int selected_gear_slot;
    struct GearSlotWidget* weapon_slot_widget;
    struct GearSlotWidget* armor_slot_widget;
    struct GearSlotWidget* shield_slot_widget;
    struct GearSlotWidget* accessory_slot_widget;
};

struct GearWidget* create_gear_widget(struct GuiScreen* gui_screen);
enum ItemType get_gear_widget_selected_item_type(struct GearWidget* widget);
struct GearSlotWidget* get_gear_widget_selected_gear_slot(struct GearWidget* widget);

void process_gear_widget_input(
    struct GearWidget* widget,
    struct InputDevice* input_device);

void reset_gear_widget(struct GearWidget* widget);

void set_gear_widget_position(
    struct GearWidget* widget,
    const int x,
    const int y);

void update_gear_widget(struct GearWidget* widget, struct Actor* actor);

#endif