#include <assert.h>
#include <stdlib.h>
#include "actor.h"
#include "direction.h"
#include "map.h"

//  ---------------------------------------------------------------------------
struct Actor* CreateActor(struct Map* map, int tileX, int tileY, int tilesetId)
{
    assert(map != NULL);

    struct Actor* actor = malloc(sizeof(struct Actor));
    actor->Collision = 1;
    actor->OnTouch = NULL;
    actor->Map = map;
    actor->MoveDirection = DIRECTION_NONE;
    actor->Tile = GetTile(map, tileX, tileY);
    actor->TilesetId = tilesetId;
    actor->Type = ACTOR_TYPE_NONE;
    actor->MaxHealth = 100;
    actor->Health = actor->MaxHealth;
    return actor;
}

//  ---------------------------------------------------------------------------
void DestroyActor(struct Actor** actor)
{
    assert(actor != NULL);

    if (*actor != NULL)
    {
        free(*actor);
        *actor = NULL;
    }
}
