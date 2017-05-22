#include "actor.h"
#include "direction.h"
#include "map.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Actor* CreateActor(
    struct Map* map,
    const char* name,
    int tileX, 
    int tileY,
    int tilesetId)
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
    actor->Name = strdup(name);
    actor->Cash = 0;
    return actor;
}

//  ---------------------------------------------------------------------------
void DestroyActor(struct Actor** actor)
{
    assert(actor != NULL);

    if (*actor != NULL)
    {
        free((*actor)->Name);
        free(*actor);
        *actor = NULL;
    }
}

//  ---------------------------------------------------------------------------
int IsFoe(struct Actor* actor, struct Actor* other)
{
    assert(actor != NULL);
    assert(other != NULL);
    
    return
        (actor->Type == ACTOR_TYPE_PLAYER && other->Type == ACTOR_TYPE_VILLAIN) ||
        (actor->Type == ACTOR_TYPE_VILLAIN && other->Type == ACTOR_TYPE_PLAYER);
}