#include <assert.h>
#include <stdlib.h>
#include "actor.h"
#include "direction.h"
#include "map.h"
#include "tile.h"

//  ---------------------------------------------------------------------------
struct Actor* CreateActor(struct Map* map, int tileX, int tileY, int tilesetId)
{
    assert(map != NULL);

    struct Actor* actor = malloc(sizeof(struct Actor));
    actor->Map = map;
    actor->MoveDirection = DIRECTION_NONE;
    actor->Tile = GetTile(map, tileX, tileY);
    actor->TilesetId = tilesetId;
    return actor;
}

//  ---------------------------------------------------------------------------
void MoveActor(struct Actor* actor)
{
    assert(actor != NULL);

    struct Tile* destTile = GetNeighbor(actor->Map, actor->Tile, actor->MoveDirection);
    if (destTile != NULL && destTile->Collision == 0)
    {
        actor->Tile = destTile;
        actor->MoveDirection = DIRECTION_NONE;
    }
}