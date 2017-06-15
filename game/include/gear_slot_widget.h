#ifndef GEAR_SLOT_WIDGET_HEADER
#define GEAR_SLOT_WIDGET_HEADER

struct GuiScreen;
struct Item;
struct ItemSlotWidget;
struct Panel;

struct GearSlotWidget
{
    int index;
    struct ItemSlotWidget* item_slot_widget;
    struct Panel* slot_panel;
};

struct GearSlotWidget* create_gear_slot_widget(
    struct GuiScreen* gui_screen,
    const char* slot_name,
    int slot_index);

void set_gear_slot_widget_position(
    struct GearSlotWidget* widget,
    const int x,
    const int y);

void update_gear_slot_widget(
    struct GearSlotWidget* gear_slot_widget,
    struct Item* item);

#endif