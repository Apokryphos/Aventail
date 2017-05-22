#include "actor.h"
#include "audio.h"
#include "inventory.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
void ActivateContainer(struct Actor* source, struct Actor* target)
{
    int playSfx = 0;

    if (target->Cash > 0)
    {
        playSfx = 1;
        source->Cash += target->Cash;
        target->Cash = 0;
    }

    if (GetInventoryItemCount(target->Inventory) > 0)
    {
        if (GiveInventoryItems(target->Inventory, source->Inventory))
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
    if (target->Cash == 0 && 
        GetInventoryItemCount(target->Inventory) == 0)
    {
        target->TilesetId = 85;
    }
}

//  ---------------------------------------------------------------------------
void ActivateDoor(struct Actor* source, struct Actor* target)
{
    if (target->Collision)
    {
        target->Collision = 0;

        switch (target->TilesetId)
        {
            case 41:
                target->TilesetId = 31;
                break;
        }
    }
}