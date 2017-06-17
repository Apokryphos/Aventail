#include "actor.h"
#include "audio.h"
#include "inventory.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
void activate_container(struct Actor* source, struct Actor* target)
{
    if (target->locked)
    {
        play_sfx(SFX_ERROR);
        return;
    }

    int play_cash_sfx = 0;

    if (target->cash > 0)
    {
        play_cash_sfx = 1;
        source->cash += target->cash;
        target->cash = 0;
    }

    if (get_inventory_item_count(target->inventory) > 0)
    {
        if (move_inventory_items(target->inventory, source->inventory))
        {
            play_cash_sfx = 1;
        }
    }

    //  Play sound effect
    if (play_cash_sfx)
    {
        play_sfx(SFX_CASH_PICKUP_01);
    }

    //  Check if container is empty and change sprite if it is
    if (target->cash == 0 &&
        get_inventory_item_count(target->inventory) == 0)
    {
        target->tileset_id = 85;
    }
}

//  ---------------------------------------------------------------------------
void activate_door(struct Actor* source, struct Actor* target)
{
    if (target->collision)
    {
        if (target->locked)
        {
            play_sfx(SFX_ERROR);
        }
        else
        {
            target->collision = 0;

            play_sfx(SFX_DOOR);

            switch (target->tileset_id)
            {
                case 41:
                    target->tileset_id = 31;
                    break;
            }
        }
    }
}