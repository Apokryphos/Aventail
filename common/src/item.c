#include "item.h"
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* CreateItem(const char* name)
{
    struct Item* item = malloc(sizeof(struct Item));
    item->Name = strdup(name);
    item->TilesetId = 91;
    item->HealAmount = 0;
    item->Stats.Attack = 0;
    item->Stats.Defend = 0;
    item->Stats.Vitality = 0;
    item->Type = ITEM_TYPE_NONE;
    return item;
}

//  ---------------------------------------------------------------------------
void DestroyItem(struct Item** item)
{
    if (*item == NULL)
    {
        free(*item);
        *item = NULL;
    }
}