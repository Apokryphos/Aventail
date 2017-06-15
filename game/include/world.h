#ifndef WORLD_HEADER_INCLUDED
#define WORLD_HEADER_INCLUDED

#include "player.h"

struct Actor;
struct Game;
struct Inventory;
struct Zone;

struct World
{
    struct Player player;
    struct Zone* zone;
};

struct Actor* create_player_actor(struct World* world);

/*
    Allocates a World. Free with destroy_world.
*/
struct World* create_world();

/**
    Frees the specified World and its ActorList and Map. Afterwards the
    specified pointer will be set to NULL.
*/
void destroy_world(struct World** world);

#endif