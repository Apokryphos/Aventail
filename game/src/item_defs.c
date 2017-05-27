#include "item.h"
#include <assert.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Item* CreateWeapon(const char* name)
{
    assert(name != NULL);

    struct Item* item = CreateItem(name);
    item->TilesetId = 130;
    item->Stats.Attack = 5;
    item->Type = ITEM_TYPE_WEAPON;

    return item;
}

//  ---------------------------------------------------------------------------
void LoadItemDefinition(struct Item* item)
{
    assert(item != NULL);

    //  Simple definitions until loading from data files is supported
    if (strcasecmp(item->Name, "bronze sword") == 0)
    {
        item->TilesetId = 130;
        item->Stats.Attack = 3;
        item->Type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->Name, "iron short sword") == 0)
    {
        item->TilesetId = 131;
        item->Stats.Attack = 9;
        item->Type = ITEM_TYPE_WEAPON;
    }
    if (strcasecmp(item->Name, "leather cuirass") == 0)
    {
        item->TilesetId = 160;
        item->Stats.Attack = 1;
        item->Stats.Defend = 2;
        item->Stats.Vitality = 12;
        item->Type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->Name, "chainmail") == 0)
    {
        item->TilesetId = 164;
        item->Stats.Defend = 6;
        item->Type = ITEM_TYPE_ARMOR;
    }
    if (strcasecmp(item->Name, "buckler") == 0)
    {
        item->TilesetId = 172;
        item->Stats.Defend = 2;
        item->Type = ITEM_TYPE_SHIELD;
    }
    else if (strcasecmp(item->Name, "health vial") == 0)
    {
        item->TilesetId = 97;
        item->HealAmount = 10;
        item->Type = ITEM_TYPE_CONSUMABLE;
    }
    else if (strcasecmp(item->Name, "healing ring") == 0)
    {
        item->TilesetId = 92;
        item->HealAmount = 1;
        item->Type = ITEM_TYPE_ACCESSORY;
    }
}