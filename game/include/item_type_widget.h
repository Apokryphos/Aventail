#ifndef ITEM_TYPE_WIDGET_HEADER
#define ITEM_TYPE_WIDGET_HEADER

#include "item_type.h"

struct GuiScreen;
struct InputDevice;
struct Panel;

struct ItemTypeWidget
{
    enum ItemType selected_item_type;
    struct Panel* item_type_panels[ITEM_TYPE_COUNT];
};

struct ItemTypeWidget* create_item_type_widget(struct GuiScreen* gui_screen);

struct Panel* get_selected_item_type_panel(struct ItemTypeWidget* widget);

void process_item_type_widget_input(
    struct ItemTypeWidget* widget,
    struct InputDevice* input_device);

void reset_item_type_widget(struct ItemTypeWidget* widget);

void set_item_type_widget_position(
    struct ItemTypeWidget* widget,
    const int x,
    const int y);

#endif