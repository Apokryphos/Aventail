#include "item.h"
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* CreateItem(const char* name)
{
    struct Item* item = malloc(sizeof(struct Item));
    item->Name = strdup(name);
    item->TilesetId = 91;
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