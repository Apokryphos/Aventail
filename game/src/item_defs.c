#include "item.h"
#include <assert.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* create_weapon(const char* name)
{
    assert(name != NULL);

    struct Item* item = create_item(name);
    item->tileset_id = 130;
    item->stats.attack = 5;
    item->type = ITEM_TYPE_WEAPON;

    return item;
}

//  ---------------------------------------------------------------------------
void load_item_definition(struct Item* item)
{
    assert(item != NULL);

    //  Simple definitions until loading from data files is supported
    if (strcasecmp(item->name, "bronze sword") == 0)
    {
        item->tileset_id = 130;
        item->stats.attack = 3;
        item->type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->name, "iron short sword") == 0)
    {
        item->tileset_id = 131;
        item->stats.attack = 9;
        item->type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->name, "leather cuirass") == 0)
    {
        item->tileset_id = 160;
        item->stats.attack = 1;
        item->stats.defend = 2;
        item->stats.vitality = 12;
        item->type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->name, "chainmail") == 0)
    {
        item->tileset_id = 164;
        item->stats.defend = 6;
        item->type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->name, "buckler") == 0)
    {
        item->tileset_id = 172;
        item->stats.defend = 2;
        item->type = ITEM_TYPE_SHIELD;
    }
    else if (strcasecmp(item->name, "health vial") == 0)
    {
        item->tileset_id = 97;
        item->heal_amount = 10;
        item->type = ITEM_TYPE_CONSUMABLE;
    }
    else if (strcasecmp(item->name, "healing ring") == 0)
    {
        item->tileset_id = 92;
        item->heal_amount = 1;
        item->type = ITEM_TYPE_ACCESSORY;
    }
}