#include "inventory.h"
#include <stdlib.h>

const size_t MaxInventoryItems = 30;

//  ---------------------------------------------------------------------------
int AddInventoryItem(struct Inventory* inventory, struct Item* item)
{
    if (item == NULL)
    {
        return 0;
    }

    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        if (inventory->Items[n] == NULL)
        {
            inventory->Items[n] = item;
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
struct Inventory* CreateInventory()
{
    struct Inventory* inventory = malloc(sizeof(struct Inventory));

    inventory->Items = malloc(sizeof(struct Item) * MaxInventoryItems);
    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        inventory->Items[n] = NULL;
    }

    return inventory;
}

//  ---------------------------------------------------------------------------
void DestroyInventory(struct Inventory** inventory)
{
    if (*inventory != NULL)
    {
        for (size_t n = 0; n < MaxInventoryItems; ++n)
        {
            if ((*inventory)->Items[n] != NULL)
            {
                DestroyItem(&(*inventory)->Items[n]);
            }
        }

        free((*inventory)->Items);
        free(*inventory);
        *inventory = NULL;
    }
}

//  ---------------------------------------------------------------------------
size_t GetInventoryItemCount(const struct Inventory* inventory)
{
    size_t count = 0;
    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        if (inventory->Items[n] != NULL)
        {
            ++count;
        }
    }
    return count;
}

//  ---------------------------------------------------------------------------
void GetInventoryItemsByType(
    const struct Inventory* inventory,
    const enum ItemType itemType,
    struct Item** items,
    size_t* count)
{
    *count = 0;
    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        items[n] = NULL;
    }

    int s = 0;
    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        if (inventory->Items[n] != NULL &&
            (itemType == ITEM_TYPE_NONE || inventory->Items[n]->Type == itemType))
        {
            items[s++] = inventory->Items[n];
            ++(*count);
        }
    }
}

//  ---------------------------------------------------------------------------
int GiveInventoryItems(struct Inventory* source, struct Inventory* dest)
{
    int gave = 0;
    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        if (source->Items[n] != NULL)
        {
            if (AddInventoryItem(dest, source->Items[n]))
            {
                source->Items[n] = NULL;
                gave = 1;
            }
        }
    }

    return gave;
}

//  ---------------------------------------------------------------------------
int InventoryIsFull(struct Inventory* inventory)
{
    return GetInventoryItemCount(inventory) >= MaxInventoryItems;
}

//  ---------------------------------------------------------------------------
int RemoveInventoryItem(struct Inventory* inventory, struct Item* item)
{
    if (item == NULL)
    {
        return 0;
    }

    for (size_t n = 0; n < MaxInventoryItems; ++n)
    {
        if (inventory->Items[n] == item)
        {
            inventory->Items[n] = NULL;
            return 1;
        }
    }

    return 0;
}