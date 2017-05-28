#include "actor.h"
#include "direction.h"
#include "inventory.h"
#include "map.h"
#include "stats.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Actor* create_actor(
    struct Map* map,
    const char* name,
    const int tile_x,
    const int tile_y,
    const int tileset_id)
{
    struct Actor* actor = malloc(sizeof(struct Actor));

    actor->name = strdup(name);
    actor->inventory = create_inventory();

    actor->map = map;

    actor->tile = NULL;
    if (map != NULL)
    {
        assert(in_map_bounds(map, tile_x, tile_y));
        actor->tile = get_map_tile(map, tile_x, tile_y);
    }

    actor->max_action_points = 1;
    actor->action_points = actor->max_action_points;
    actor->collision = 1;
    actor->on_touch = NULL;
    actor->move_direction = DIRECTION_NONE;
    actor->tileset_id = tileset_id;
    actor->type = ACTOR_TYPE_NONE;
    actor->max_health = 100;
    actor->health = actor->max_health;
    actor->cash = 0;
    actor->stats.Attack = 0;
    actor->stats.Defend = 0;
    actor->stats.Vitality = 0;

    actor->gear = (struct Gear) { 0 };

    return actor;
}

//  ---------------------------------------------------------------------------
void destroy_actor(struct Actor** actor)
{
    assert(actor != NULL);

    if (*actor != NULL)
    {
        remove_all_items_from_gear(*actor);
        destroy_inventory(&(*actor)->inventory);
        free((*actor)->name);
        free(*actor);
        *actor = NULL;
    }
}

//  ---------------------------------------------------------------------------
int is_actor_dead(const struct Actor* actor)
{
    assert(actor != NULL);
    return actor->health <= 0;
}

//  ---------------------------------------------------------------------------
int is_actor_foe(const struct Actor* actor, const struct Actor* other)
{
    assert(actor != NULL);
    assert(other != NULL);

    return
        (actor->type == ACTOR_TYPE_PLAYER && other->type == ACTOR_TYPE_VILLAIN) ||
        (actor->type == ACTOR_TYPE_VILLAIN && other->type == ACTOR_TYPE_PLAYER);
}