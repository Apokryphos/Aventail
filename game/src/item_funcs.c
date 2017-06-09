#include "actor.h"
#include "audio.h"
#include "item.h"
#include "inventory.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
int use_heal_item(struct Item* item, struct Actor* target)
{
    //  Only consume if target actor isn't at full health
    if (target->health < target->max_health)
    {
        play_sfx(SFX_HEAL);

        target->health += item->heal_amount;
        if (target->health > target->max_health)
        {
            target->health = target->max_health;
        }

        remove_item_from_inventory(target->inventory, item);

        return 0;
    }
    else
    {
        return 1;
    }
}