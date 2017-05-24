#ifndef WORLD_HEADER_INCLUDED
#define WORLD_HEADER_INCLUDED

#include "player.h"

struct Actor;
struct ActorList;
struct Game;
struct Inventory;
struct Map;

struct World
{
    struct Map* Map;
    struct ActorList* Actors;
    struct Player Player;
};

struct Actor* CreatePlayerActor(struct World* world);
/*
    Allocates a World. Free with DestroyWorld.
*/
struct World* CreateWorld();
/**
    Frees the specified World and its ActorList and Map. Afterwards the
    specified pointer will be set to NULL.
*/
void DestroyWorld(struct World** world);
void SimulateWorld(struct Game* game, struct World* world);

#endif