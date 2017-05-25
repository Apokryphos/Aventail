#include "item.h"
#include <string.h>

//  ---------------------------------------------------------------------------
void LoadItemDefinition(struct Item* item)
{
    //  Simple definitions until loading from data files is supported
    if (strcasecmp(item->Name, "bronze sword") == 0)
    {
        item->TilesetId = 130;
        item->Stats.Attack = 3;
    }
    else if (strcasecmp(item->Name, "health vial") == 0)
    {
        item->TilesetId = 97;
        item->HealAmount = 10;
    }
}