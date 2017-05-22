#include "actor.h"
#include "audio.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
void ActivateContainer(struct Actor* source, struct Actor* target)
{
    target->TilesetId = 85;

    if (target->Cash > 0)
    {
        PlaySfx(SFX_CASH_PICKUP_01);
        source->Cash += target->Cash;
        target->Cash = 0;
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