#ifndef ACTOR_HEADER_INCLUDED
#define ACTOR_HEADER_INCLUDED

#include "actor_type.h"
#include "direction.h"

struct Actor;
struct Map;
struct Tile;
struct World;

typedef void ActorOnTouchFunction(struct Actor* source, struct Actor* target);

struct Actor
{
    enum Direction MoveDirection;
    enum ActorType Type;
    int ActionPoints;
    int MaxActionPoints;
    int Collision;
    int TilesetId;
    int Health;
    int MaxHealth;
    int Cash;
    char* Name;
    ActorOnTouchFunction* OnTouch;
    struct Inventory* Inventory;
    struct Map* Map;
    struct Tile* Tile;
};

int CanAct(struct Actor* actor);
struct Actor* CreateActor(
    struct Map* map,
    const char *name,
    int tileX,
    int tileY, 
    int tilesetId);
void DestroyActor(struct Actor** actor);
int IsFoe(struct Actor* actor, struct Actor* other);

#endif