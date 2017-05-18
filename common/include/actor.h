#ifndef ACTOR_HEADER_INCLUDED
#define ACTOR_HEADER_INCLUDED

#include "direction.h"

struct Map;
struct Tile;

struct Actor
{
    int TilesetId;
    enum Direction MoveDirection;
    struct Map* Map;
    struct Tile* Tile;
};

struct Actor* CreateActor(struct Map* map, int tileX, int tileY, int tilesetId);
void MoveActor(struct Actor* actor);

#endif