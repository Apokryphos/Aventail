#ifndef WORLD_HEADER_INCLUDED
#define WORLD_HEADER_INCLUDED

#include "actor_list.h"
#include "player.h"

struct Actor;
struct Map;
struct MapLink;

struct World
{
    struct MapLink* LoadMapLink;
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
void SimulateWorld(struct World* world);

#endif