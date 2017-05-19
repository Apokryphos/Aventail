#include "actor.h"
#include <stdio.h>

//  ---------------------------------------------------------------------------
void ActivateDoor(struct Actor* source, struct Actor* target)
{
    if (target->Collision)
    {
        target->Collision = 0;

        printf("ActivateDoor %d", target->TilesetId);
        switch (target->TilesetId)
        {
            case 41:
                target->TilesetId = 31;
                break;
        }
    }
}