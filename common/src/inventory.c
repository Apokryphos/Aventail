#include "inventory.h"
#include <stdlib.h>

const size_t MAX_INVENTORY_ITEMS = 30;

//  ---------------------------------------------------------------------------
int add_item_to_inventory(struct Inventory* inventory, struct Item* item)
{
    if (item == NULL)
    {
        return 0;
    }

    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        if (inventory->items[n] == NULL)
        {
            inventory->items[n] = item;
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
struct Inventory* create_inventory()
{
    struct Inventory* inventory = malloc(sizeof(struct Inventory));

    inventory->items = malloc(sizeof(struct Item) * MAX_INVENTORY_ITEMS);
    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        inventory->items[n] = NULL;
    }

    return inventory;
}

//  ---------------------------------------------------------------------------
void destroy_inventory(struct Inventory** inventory)
{
    if (*inventory != NULL)
    {
        for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
        {
            if ((*inventory)->items[n] != NULL)
            {
                destroy_item(&(*inventory)->items[n]);
            }
        }

        free((*inventory)->items);
        free(*inventory);
        *inventory = NULL;
    }
}

//  ---------------------------------------------------------------------------
size_t get_inventory_item_count(const struct Inventory* inventory)
{
    size_t count = 0;
    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        if (inventory->items[n] != NULL)
        {
            ++count;
        }
    }
    return count;
}

//  ---------------------------------------------------------------------------
void get_inventory_items_by_item_type(
    const struct Inventory* inventory,
    const enum ItemType item_type,
    struct Item** items,
    size_t* count)
{
    *count = 0;
    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        items[n] = NULL;
    }

    int s = 0;
    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        if (inventory->items[n] != NULL &&
            (item_type == ITEM_TYPE_NONE || inventory->items[n]->type == item_type))
        {
            items[s++] = inventory->items[n];
            ++(*count);
        }
    }
}

//  ---------------------------------------------------------------------------
int move_inventory_items(struct Inventory* source, struct Inventory* dest)
{
    int gave = 0;
    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        if (source->items[n] != NULL)
        {
            if (add_item_to_inventory(dest, source->items[n]))
            {
                source->items[n] = NULL;
                gave = 1;
            }
        }
    }

    return gave;
}

//  ---------------------------------------------------------------------------
int is_inventory_full(struct Inventory* inventory)
{
    return get_inventory_item_count(inventory) >= MAX_INVENTORY_ITEMS;
}

//  ---------------------------------------------------------------------------
int remove_item_from_inventory(struct Inventory* inventory, struct Item* item)
{
    if (item == NULL)
    {
        return 0;
    }

    for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
    {
        if (inventory->items[n] == item)
        {
            inventory->items[n] = NULL;
            return 1;
        }
    }

    return 0;
}