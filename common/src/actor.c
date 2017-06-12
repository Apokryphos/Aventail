#include "actor.h"
#include "actor_ai.h"
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
    const int tileset_id,
    const int flip_flags)
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
    actor->draw_order = 0;
    actor->flip_flags = flip_flags;
    actor->rotation = 0;
    actor->ai = NULL;
    actor->on_touch = NULL;
    actor->move_direction = DIRECTION_NONE;
    actor->tileset_id = tileset_id;
    actor->type = ACTOR_TYPE_NONE;
    actor->max_health = 100;
    actor->health = actor->max_health;
    actor->cash = 0;
    actor->stats.attack = 0;
    actor->stats.defend = 0;
    actor->stats.vitality = 0;

    actor->gear = (struct Gear) { 0 };

    return actor;
}

//  ---------------------------------------------------------------------------
void destroy_actor(struct Actor** actor)
{
    assert(actor != NULL);

    if (*actor != NULL)
    {
        if ((*actor)->ai != NULL)
        {
            destroy_actor_ai(&(*actor)->ai);
        }

        remove_all_items_from_gear(*actor);
        destroy_inventory(&(*actor)->inventory);
        free((*actor)->name);
        free(*actor);
        *actor = NULL;
    }
}

//  ---------------------------------------------------------------------------
int get_actor_draw_order(const struct Actor* actor)
{
    switch (actor->type)
    {
        default:
        case ACTOR_TYPE_NONE:
        case ACTOR_TYPE_DOOR:
        case ACTOR_TYPE_CONTAINER:
            return 0;

        case ACTOR_TYPE_VILLAIN:
            return is_actor_dead(actor) ? 1 : 2;

        case ACTOR_TYPE_PLAYER:
            return is_actor_dead(actor) ? 1 : 3;
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