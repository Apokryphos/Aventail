#include "item.h"
#include <assert.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* CreateWeapon(const char* name)
{
    assert(name != NULL);

    struct Item* item = create_item(name);
    item->tileset_id = 130;
    item->stats.Attack = 5;
    item->type = ITEM_TYPE_WEAPON;

    return item;
}

//  ---------------------------------------------------------------------------
void LoadItemDefinition(struct Item* item)
{
    assert(item != NULL);

    //  Simple definitions until loading from data files is supported
    if (strcasecmp(item->name, "bronze sword") == 0)
    {
        item->tileset_id = 130;
        item->stats.Attack = 3;
        item->type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->name, "iron short sword") == 0)
    {
        item->tileset_id = 131;
        item->stats.Attack = 9;
        item->type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->name, "leather cuirass") == 0)
    {
        item->tileset_id = 160;
        item->stats.Attack = 1;
        item->stats.Defend = 2;
        item->stats.Vitality = 12;
        item->type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->name, "chainmail") == 0)
    {
        item->tileset_id = 164;
        item->stats.Defend = 6;
        item->type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->name, "buckler") == 0)
    {
        item->tileset_id = 172;
        item->stats.Defend = 2;
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