#include "item.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* create_item(const char* name)
{
    struct Item* item = malloc(sizeof(struct Item));
    item->name = strdup(name);
    item->tileset_id = 91;
    item->heal_amount = 0;
    item->stats.attack = 0;
    item->stats.defend = 0;
    item->stats.vitality = 0;
    item->type = ITEM_TYPE_NONE;
    item->on_use = NULL;
    return item;
}

//  ---------------------------------------------------------------------------
void destroy_item(struct Item** item)
{
    assert(item != NULL);

    if (*item != NULL)
    {
        if ((*item)->name != NULL)
        {
            free((*item)->name);
        }

        free(*item);
        *item = NULL;
    }
}