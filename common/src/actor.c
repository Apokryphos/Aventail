#include "actor.h"
#include "direction.h"
#include "inventory.h"
#include "map.h"
#include "stats.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Actor* CreateActor(
    struct Map* map,
    const char* name,
    const int tileX, 
    const int tileY,
    const int tilesetId)
{
    struct Actor* actor = malloc(sizeof(struct Actor));

    actor->Name = strdup(name);
    actor->Inventory = CreateInventory();
    
    actor->Map = map;

    actor->Tile = NULL;
    if (map != NULL)
    {
        assert(InBounds(map, tileX, tileY));
        actor->Tile = GetTile(map, tileX, tileY);
    }

    actor->MaxActionPoints = 1;
    actor->ActionPoints = actor->MaxActionPoints;
    actor->Collision = 1;
    actor->OnTouch = NULL;
    actor->MoveDirection = DIRECTION_NONE;
    actor->TilesetId = tilesetId;
    actor->Type = ACTOR_TYPE_NONE;
    actor->MaxHealth = 100;
    actor->Health = actor->MaxHealth;
    actor->Cash = 0;
    actor->Stats.Attack = 0;
    actor->Stats.Defend = 0;
    actor->Stats.Vitality = 0;

    actor->Gear = (struct Gear) { 0 };

    return actor;
}

//  ---------------------------------------------------------------------------
void DestroyActor(struct Actor** actor)
{
    assert(actor != NULL);

    if (*actor != NULL)
    {
        RemoveAllGearItems(*actor);
        DestroyInventory(&(*actor)->Inventory);
        free((*actor)->Name);
        free(*actor);
        *actor = NULL;
    }
}

//  ---------------------------------------------------------------------------
int ActorIsDead(const struct Actor* actor)
{
    assert(actor != NULL);
    return actor->Health <= 0;
}

//  ---------------------------------------------------------------------------
int ActorIsFoe(const struct Actor* actor, const struct Actor* other)
{
    assert(actor != NULL);
    assert(other != NULL);
    
    return
        (actor->Type == ACTOR_TYPE_PLAYER && other->Type == ACTOR_TYPE_VILLAIN) ||
        (actor->Type == ACTOR_TYPE_VILLAIN && other->Type == ACTOR_TYPE_PLAYER);
}