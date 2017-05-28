#include "gui_screen.h"
#include "inventory.h"
#include "inventory_widget.h"
#include "item_slot_widget.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>

static const int ARROW_ICON_TILESET_ID = 1085;

//  ---------------------------------------------------------------------------
struct InventoryWidget* create_inventory_widget(struct GuiScreen* gui_screen)
{
    struct InventoryWidget* widget = malloc(sizeof(struct InventoryWidget));

    widget->item_type = ITEM_TYPE_NONE;
    widget->item_count = 0;
    widget->selected_item_index = 0;
    widget->selected_item_slot_widget = NULL;

    widget->panel = CreatePanel("Inventory", PANEL_BORDER_STYLE_2);
    widget->panel->Width = 196;
    widget->panel->Height = INVENTORY_WIDGET_MAX_VISIBLE_ITEMS * 40;
    widget->panel->Background = 1;
    AddGuiScreenPanel(gui_screen, widget->panel);

    widget->items = malloc(sizeof(struct Item*) * MAX_INVENTORY_ITEMS);
    for (int n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        widget->items[n] = NULL;
    }

    for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
    {
        widget->item_slot_widgets[p] = CreateItemSlotWidget(gui_screen);
    }

    widget->bottom_scroll_arrow_panel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->bottom_scroll_arrow_panel->Width = 8;
    widget->bottom_scroll_arrow_panel->Height = 8;
    widget->bottom_scroll_arrow_panel->Icon.Flip = FLIP_FLAG_VERT;
    widget->bottom_scroll_arrow_panel->Icon.TilesetId = ARROW_ICON_TILESET_ID;
    widget->bottom_scroll_arrow_panel->Icon.Style = PANEL_ICON_STYLE_SMALL;
    AddGuiScreenPanel(gui_screen, widget->bottom_scroll_arrow_panel);

    widget->top_scroll_arrow_panel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->top_scroll_arrow_panel->Width = 8;
    widget->top_scroll_arrow_panel->Height = 8;
    widget->top_scroll_arrow_panel->Icon.TilesetId = ARROW_ICON_TILESET_ID;
    widget->top_scroll_arrow_panel->Icon.Style = PANEL_ICON_STYLE_SMALL;
    AddGuiScreenPanel(gui_screen, widget->top_scroll_arrow_panel);

    set_inventory_widget_position(widget, 0, 0);

    return widget;
}

//  ---------------------------------------------------------------------------
void destroy_inventory_widget(struct InventoryWidget** widget)
{
    assert(widget != NULL);

    if (*widget != NULL)
    {
        if ((*widget)->panel != NULL)
        {
            DestroyPanel(&(*widget)->panel);
        }
        for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
        {
            DestroyItemSlotWidget(&(*widget)->item_slot_widgets[p]);
        }
        free((*widget)->items);

        free(*widget);
        *widget = NULL;
    }
}

//  ---------------------------------------------------------------------------
void select_next_inventory_widget_item_slot(struct InventoryWidget* widget)
{
    if (widget->item_count > 0)
    {
        if (widget->selected_item_index < widget->item_count - 1)
        {
            ++widget->selected_item_index;
        }
        else
        {
            widget->selected_item_index = 0;
        }
    }
}

//  ---------------------------------------------------------------------------
void select_previous_inventory_widget_item_slot(struct InventoryWidget* widget)
{
    if (widget->item_count > 0)
    {
        if (widget->selected_item_index > 0)
        {
            --widget->selected_item_index;
        }
        else
        {
            widget->selected_item_index = widget->item_count - 1;
        }
    }
}

//  ---------------------------------------------------------------------------
void set_inventory_widget_position(struct InventoryWidget* widget, int x, int y)
{
    widget->panel->X = x;
    widget->panel->Y = y;

    widget->bottom_scroll_arrow_panel->X =
        widget->panel->X +
        widget->panel->Width / 2 -
        widget->bottom_scroll_arrow_panel->Width / 2;

    widget->bottom_scroll_arrow_panel->Y =
        widget->panel->Y +
        widget->panel->Height -
        widget->bottom_scroll_arrow_panel->Height * 2 + 2;

    widget->top_scroll_arrow_panel->X =
        widget->panel->X +
        widget->panel->Width / 2 -
        widget->top_scroll_arrow_panel->Width / 2;

    widget->top_scroll_arrow_panel->Y =
        widget->panel->Y +
        widget->top_scroll_arrow_panel->Height - 2;

    const int itemPanelHeight = 32;
    for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
    {
        SetItemSlotWidgetPosition(
            widget->item_slot_widgets[p],
            x,
            y + (p * itemPanelHeight) + (p * 4));
    }
}

//  ---------------------------------------------------------------------------
void update_inventory_widget(
    struct InventoryWidget* widget,
    struct Inventory* inventory)
{
    get_inventory_items_by_item_type(
        inventory,
        widget->item_type,
        widget->items,
        &widget->item_count);

    //  Hide all of the item slot widgets
    for (int s = 0; s < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++s)
    {
        SetItemSlotWidgetVisible(widget->item_slot_widgets[s], 0);
    }

    if (widget->item_count > 0)
    {
        //  Position
        int p = widget->selected_item_index;
        //  Visible range
        int v = INVENTORY_WIDGET_MAX_VISIBLE_ITEMS;
        //  Visible range mid-point (halfway)
        int h = v / 2;
        //  Item count
        int n = widget->item_count;

        int start, end;

        if (p <= h || n <= v)
        {
            //  Drawing the first v items because the position is near the
            //  start or the total number of items is less-than or equal-to
            //  the max number of visible items.
            start = 0;
            end = n < v ? n : v;

            widget->top_scroll_arrow_panel->Visible = 0;
            widget->bottom_scroll_arrow_panel->Visible = end < n;
        }
        else if (p >= n - h)
        {
            //  Drawing the last v items because the position is close to
            //  the end and scrolling should be avoided.
            start = n - v;
            end = n;

            widget->top_scroll_arrow_panel->Visible = start > 0;
            widget->bottom_scroll_arrow_panel->Visible = 0;
        }
        else
        {
            //  Drawing the items between start and end with items
            //  available for scrolling both above and below
            start = p - h;
            end = p + h + 1;

            widget->top_scroll_arrow_panel->Visible = 1;
            widget->bottom_scroll_arrow_panel->Visible = 1;
        }

        // Update the currently selected ItemSlotWidget
        widget->selected_item_slot_widget = widget->item_slot_widgets[p - start];

        assert(start >= 0);
        assert(start <= end);
        assert(end <= MAX_INVENTORY_ITEMS);

        //  Iterate through the widget's visible Items and update ItemSlotWidgets
        int s = 0;
        for (int n = start; n < end; ++n)
        {
            struct Item* item = widget->items[n];
            assert(item != NULL);
            if (s < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS)
            {
                SetItemSlotWidgetVisible(widget->item_slot_widgets[s], 1);
                UpdateItemSlotWidget(widget->item_slot_widgets[s], item);
                ++s;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        widget->selected_item_slot_widget = NULL;

        widget->top_scroll_arrow_panel->Visible = 0;
        widget->bottom_scroll_arrow_panel->Visible = 0;
    }
}