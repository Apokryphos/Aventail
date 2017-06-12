#ifndef ACTOR_HEADER_INCLUDED
#define ACTOR_HEADER_INCLUDED

#include "actor_type.h"
#include "direction.h"
#include "gear.h"
#include "stats.h"

struct Actor;
struct ActorAi;
struct Map;
struct Tile;
struct World;

typedef void ActorOnTouchFunction(struct Actor* source, struct Actor* target);

struct Actor
{
    enum Direction move_direction;
    enum ActorType type;
    int collision;
    int draw_order;
    int flip_flags;
    int tileset_id;
    int action_points;
    int max_action_points;
    int health;
    int max_health;
    int cash;
    double rotation;
    char* name;
    ActorOnTouchFunction* on_touch;
    struct ActorAi* ai;
    struct Inventory* inventory;
    struct Map* map;
    struct Tile* tile;
    struct Stats stats;
    struct Gear gear;
};

struct Actor* create_actor(
    struct Map* map,
    const char *name,
    const int tile_x,
    const int tile_y,
    const int tileset_id,
    const int flip_flags);
void destroy_actor(struct Actor** actor);
int get_actor_draw_order(const struct Actor* actor);
int is_actor_dead(const struct Actor* actor);
int is_actor_foe(const struct Actor* actor, const struct Actor* other);

#endif