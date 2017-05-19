#ifndef ACTOR_HEADER_INCLUDED
#define ACTOR_HEADER_INCLUDED

#include "actor_type.h"
#include "direction.h"

struct Actor;
struct Map;
struct Tile;

typedef void ActorOnTouchFunction(struct Actor* source, struct Actor* target);

struct Actor
{
    int Collision;
    int TilesetId;
    ActorOnTouchFunction *OnTouch;
    enum Direction MoveDirection;
    enum ActorType Type;
    struct Map* Map;
    struct Tile* Tile;
    int Health;
    int MaxHealth;
};

struct Actor* CreateActor(struct Map* map, int tileX, int tileY, int tilesetId);
void DestroyActor(struct Actor** actor);

#endif