#include "actor.h"
#include "audio.h"
#include "inventory.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
void ActivateContainer(struct Actor* source, struct Actor* target)
{
    int playSfx = 0;

    if (target->cash > 0)
    {
        playSfx = 1;
        source->cash += target->cash;
        target->cash = 0;
    }

    if (GetInventoryItemCount(target->inventory) > 0)
    {
        if (GiveInventoryItems(target->inventory, source->inventory))
        {
            playSfx = 1;
        }
    }

    //  Play sound effect
    if (playSfx)
    {
        PlaySfx(SFX_CASH_PICKUP_01);
    }

    //  Check if container is empty and change sprite if it is
    if (target->cash == 0 &&
        GetInventoryItemCount(target->inventory) == 0)
    {
        target->tileset_id = 85;
    }
}

//  ---------------------------------------------------------------------------
void ActivateDoor(struct Actor* source, struct Actor* target)
{
    if (target->collision)
    {
        target->collision = 0;

        PlaySfx(SFX_DOOR);

        switch (target->tileset_id)
        {
            case 41:
                target->tileset_id = 31;
                break;
        }
    }
}