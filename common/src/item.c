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
    item->stats.Attack = 0;
    item->stats.Defend = 0;
    item->stats.Vitality = 0;
    item->type = ITEM_TYPE_NONE;
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