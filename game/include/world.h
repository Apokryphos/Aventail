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
    struct Map* map;
    struct ActorList* actors;
    struct Player player;
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
void simulate_world(struct Game* game, struct World* world);

#endif