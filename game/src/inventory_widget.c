#include "gui_screen.h"
#include "inventory.h"
#include "inventory_widget.h"
#include "item_slot_widget.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>

static const int ArrowIconTilesetId = 1085;

//  ---------------------------------------------------------------------------
struct InventoryWidget* CreateInventoryWidget(struct GuiScreen* guiScreen)
{
    struct InventoryWidget* widget = malloc(sizeof(struct InventoryWidget));

    widget->ItemType = ITEM_TYPE_NONE;
    widget->ItemCount = 0;
    widget->SelectedItemIndex = 0;
    widget->SelectedItemSlotWidget = NULL;

    widget->Panel = CreatePanel("Inventory", PANEL_BORDER_STYLE_2);
    widget->Panel->Width = 196;
    widget->Panel->Height = INVENTORY_WIDGET_MAX_VISIBLE_ITEMS * 40;
    widget->Panel->Background = 1;
    AddGuiScreenPanel(guiScreen, widget->Panel);

    widget->Items = malloc(sizeof(struct Item*) * MAX_INVENTORY_ITEMS);
    for (int n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        widget->Items[n] = NULL;
    }

    for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
    {
        widget->ItemSlotWidgets[p] = CreateItemSlotWidget(guiScreen);
    }

    widget->ScrollArrowBottomPanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->ScrollArrowBottomPanel->Width = 8;
    widget->ScrollArrowBottomPanel->Height = 8;
    widget->ScrollArrowBottomPanel->Icon.Flip = FLIP_FLAG_VERT;
    widget->ScrollArrowBottomPanel->Icon.TilesetId = ArrowIconTilesetId;
    widget->ScrollArrowBottomPanel->Icon.Style = PANEL_ICON_STYLE_SMALL;
    AddGuiScreenPanel(guiScreen, widget->ScrollArrowBottomPanel);

    widget->ScrollArrowTopPanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->ScrollArrowTopPanel->Width = 8;
    widget->ScrollArrowTopPanel->Height = 8;
    widget->ScrollArrowTopPanel->Icon.TilesetId = ArrowIconTilesetId;
    widget->ScrollArrowTopPanel->Icon.Style = PANEL_ICON_STYLE_SMALL;
    AddGuiScreenPanel(guiScreen, widget->ScrollArrowTopPanel);

    SetInventoryWidgetPosition(widget, 0, 0);

    return widget;
}

//  ---------------------------------------------------------------------------
void DestroyInventoryWidget(struct InventoryWidget** widget)
{
    assert(widget != NULL);

    if (*widget != NULL)
    {
        if ((*widget)->Panel != NULL)
        {
            DestroyPanel(&(*widget)->Panel);
        }
        for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
        {
            DestroyItemSlotWidget(&(*widget)->ItemSlotWidgets[p]);
        }
        free((*widget)->Items);

        free(*widget);
        *widget = NULL;
    }
}

//  ---------------------------------------------------------------------------
void SelectNextInventoryWidgetItemSlot(struct InventoryWidget* widget)
{
    if (widget->ItemCount > 0)
    {
        if (widget->SelectedItemIndex < widget->ItemCount - 1)
        {
            ++widget->SelectedItemIndex;
        }
        else
        {
            widget->SelectedItemIndex = 0;
        }
    }
}

//  ---------------------------------------------------------------------------
void SelectPreviousInventoryWidgetItemSlot(struct InventoryWidget* widget)
{
    if (widget->ItemCount > 0)
    {
        if (widget->SelectedItemIndex > 0)
        {
            --widget->SelectedItemIndex;
        }
        else
        {
            widget->SelectedItemIndex = widget->ItemCount - 1;
        }
    }
}

//  ---------------------------------------------------------------------------
void SetInventoryWidgetPosition(struct InventoryWidget* widget, int x, int y)
{
    widget->Panel->X = x;
    widget->Panel->Y = y;

    widget->ScrollArrowBottomPanel->X =
        widget->Panel->X +
        widget->Panel->Width / 2 -
        widget->ScrollArrowBottomPanel->Width / 2;

    widget->ScrollArrowBottomPanel->Y =
        widget->Panel->Y +
        widget->Panel->Height -
        widget->ScrollArrowBottomPanel->Height * 2 + 2;

    widget->ScrollArrowTopPanel->X =
        widget->Panel->X +
        widget->Panel->Width / 2 -
        widget->ScrollArrowTopPanel->Width / 2;

    widget->ScrollArrowTopPanel->Y =
        widget->Panel->Y +
        widget->ScrollArrowTopPanel->Height - 2;

    const int itemPanelHeight = 32;
    for (int p = 0; p < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++p)
    {
        SetItemSlotWidgetPosition(
            widget->ItemSlotWidgets[p],
            x,
            y + (p * itemPanelHeight) + (p * 4));
    }
}

//  ---------------------------------------------------------------------------
void UpdateInventoryWidget(
    struct InventoryWidget* widget,
    struct Inventory* inventory)
{
    get_inventory_items_by_item_type(
        inventory,
        widget->ItemType,
        widget->Items,
        &widget->ItemCount);

    //  Hide all of the item slot widgets
    for (int s = 0; s < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS; ++s)
    {
        SetItemSlotWidgetVisible(widget->ItemSlotWidgets[s], 0);
    }

    if (widget->ItemCount > 0)
    {
        //  Position
        int p = widget->SelectedItemIndex;
        //  Visible range
        int v = INVENTORY_WIDGET_MAX_VISIBLE_ITEMS;
        //  Visible range mid-point (halfway)
        int h = v / 2;
        //  Item count
        int n = widget->ItemCount;

        int start, end;

        if (p <= h || n <= v)
        {
            //  Drawing the first v items because the position is near the
            //  start or the total number of items is less-than or equal-to
            //  the max number of visible items.
            start = 0;
            end = n < v ? n : v;

            widget->ScrollArrowTopPanel->Visible = 0;
            widget->ScrollArrowBottomPanel->Visible = end < n;
        }
        else if (p >= n - h)
        {
            //  Drawing the last v items because the position is close to
            //  the end and scrolling should be avoided.
            start = n - v;
            end = n;

            widget->ScrollArrowTopPanel->Visible = start > 0;
            widget->ScrollArrowBottomPanel->Visible = 0;
        }
        else
        {
            //  Drawing the items between start and end with items
            //  available for scrolling both above and below
            start = p - h;
            end = p + h + 1;

            widget->ScrollArrowTopPanel->Visible = 1;
            widget->ScrollArrowBottomPanel->Visible = 1;
        }

        // Update the currently selected ItemSlotWidget
        widget->SelectedItemSlotWidget = widget->ItemSlotWidgets[p - start];

        assert(start >= 0);
        assert(start <= end);
        assert(end <= MAX_INVENTORY_ITEMS);

        //  Iterate through the widget's visible Items and update ItemSlotWidgets
        int s = 0;
        for (int n = start; n < end; ++n)
        {
            struct Item* item = widget->Items[n];
            assert(item != NULL);
            if (s < INVENTORY_WIDGET_MAX_VISIBLE_ITEMS)
            {
                SetItemSlotWidgetVisible(widget->ItemSlotWidgets[s], 1);
                UpdateItemSlotWidget(widget->ItemSlotWidgets[s], item);
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
        widget->SelectedItemSlotWidget = NULL;

        widget->ScrollArrowTopPanel->Visible = 0;
        widget->ScrollArrowBottomPanel->Visible = 0;
    }
}