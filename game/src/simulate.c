#include "actor.h"
#include "actor_ai.h"
#include "actor_list.h"
#include "audio.h"
#include "game.h"
#include "game_state_load_map.h"
#include "game_state_transition.h"
#include "inventory.h"
#include "map.h"
#include "path_finder.h"
#include "point.h"
#include "tile.h"
#include "world.h"
#include "vision.h"
#include "zone.h"
#include <assert.h>
#include <stdlib.h>

#define MAX_TARGET_CANIDATES 16

static int target_canidate_count = 0;
static struct Actor* target_canidates[MAX_TARGET_CANIDATES];

static struct Actor* active_actor = NULL;

//  ---------------------------------------------------------------------------
static struct Actor* create_loot_container(
    struct World* world,
    const int x,
    const int y,
    struct Inventory* inventory)
{
    assert(world->zone->map != NULL);
    assert(inventory != NULL);

    struct Actor* actor = create_actor(world->zone->map, "Loot", x, y, 91, 0);
    actor->type = ACTOR_TYPE_CONTAINER;
    add_actor_to_actor_list_front(world->zone->actors, actor);

    move_inventory_items(inventory, actor->inventory);

    return actor;
}

//  ---------------------------------------------------------------------------
static void loot_actor(
    struct Actor* source,
    struct Actor* target,
    struct World* world)
{
    if (target->cash > 0)
    {
        source->cash += target->cash;
        target->cash = 0;
    }

    if (get_inventory_item_count(target->inventory) > 0)
    {
        move_inventory_items(target->inventory, source->inventory);

        if (get_inventory_item_count(target->inventory) > 0)
        {
            create_loot_container(world, target->tile->x, target->tile->y, target->inventory);
        }
    }
}

//  ---------------------------------------------------------------------------
static void attack_actor(
    struct Actor* source,
    struct Actor* target,
    struct World* world)
{
    assert(source->action_points > 0);

    if (is_actor_alive(source) && is_actor_alive(target))
    {
        play_sfx(SFX_ATTACK_01);

        --source->action_points;

        struct Stats source_stats = add_stats(source->stats, source->gear.stats);
        struct Stats target_stats = add_stats(target->stats, target->gear.stats);

        int damage = 1;
        if (source_stats.attack > target_stats.defend)
        {
            damage = (source_stats.attack - target_stats.defend) + 1;
        }

        target->health -= damage;

        if (is_actor_dead(target))
        {
            target->collision = 0;
            target->health = 0;
            target->move_direction = DIRECTION_NONE;
            //target->tileset_id = BONES_TILESET_ID;

            loot_actor(source, target, world);
        }
        else
        {
            if (target->ai->enabled)
            {
                //  AI should target attacker
                target->ai->target = source;
            }
        }
    }
}

//  ---------------------------------------------------------------------------
static int actor_can_act(const struct Actor* actor)
{
    return
        (actor->type == ACTOR_TYPE_PLAYER ||
        actor->type == ACTOR_TYPE_VILLAIN) &&
        actor->health > 0 &&
        actor->action_points > 0;
}

//  ---------------------------------------------------------------------------
static void move_actor(
    struct Actor* actor,
    struct Game* game,
    struct World* world)
{
    assert(actor->action_points > 0);

    if (actor->move_direction == DIRECTION_NONE)
    {
        return;
    }

    struct Actor* target = NULL;

    struct Tile* dest_tile = get_tile_neighbor(
        actor->map,
        actor->tile,
        actor->move_direction);

    //  Reset move direction
    actor->move_direction = DIRECTION_NONE;

    //  Check if destination tile is valid
    if (dest_tile != NULL && dest_tile->collision == 0)
    {
        int can_move = 1;

        struct ActorListNode* other_actor_node = world->zone->actors->front;
        while (other_actor_node != NULL)
        {
            struct Actor* other_actor = other_actor_node->actor;

            //  Check if another actor occupies destination tile
            //  and is collidable
            if (other_actor->tile == dest_tile &&
                other_actor != actor)
            {
                if (other_actor->collision)
                {
                    //  Attack first foe on tile
                    if (target == NULL &&
                        is_actor_foe(actor, other_actor))
                    {
                        target = other_actor;
                    }

                    can_move = 0;
                }

                if (other_actor->on_touch != NULL)
                {
                    (*other_actor->on_touch)(actor, other_actor);
                }
            }
            other_actor_node = other_actor_node->next;
        }

        //  Attack target if any
        if (target != NULL)
        {
            attack_actor(actor, target, world);
        }
        //  Move actor to destination tile
        else if (can_move)
        {
            --actor->action_points;

            if (dest_tile->link == NULL)
            {
                actor->tile = dest_tile;
            }
            else
            {
                //  Map links are currently usable by player only
                if (actor == world->player.actor)
                {
                    active_actor = NULL;
                    queue_map_link_load(dest_tile->link);
                    begin_game_state_transition(game, GAME_STATE_LOAD_MAP);
                    return;
                }
            }
        }
    }
}

//  ---------------------------------------------------------------------------
static void next_active_actor(struct World* world)
{
    active_actor = NULL;

    struct ActorListNode* actor_node = world->zone->actors->front;
    while (actor_node != NULL)
    {
        struct Actor* actor = actor_node->actor;

        if (actor_can_act(actor))
        {
            active_actor = actor;
            return;
        }

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
static void reset_action_points(struct World* world)
{
    struct ActorListNode* actor_node = world->zone->actors->front;
    while (actor_node != NULL)
    {
        struct Actor* actor = actor_node->actor;

        actor->action_points = actor->max_action_points;

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
void revive_actor(struct Actor* actor)
{
    actor->health = actor->max_health;
    actor->collision = 1;
}

//  ---------------------------------------------------------------------------
void respawn_player_actor(struct Game* game)
{
    struct Actor* player_actor = game->world->player.actor;

    //  Inventory and gear are not currently being reset
    revive_actor(player_actor);

    //  Reset turn
    reset_action_points(game->world);
    active_actor = player_actor;

    //  Return to first map
    queue_map_load("surface01");
    begin_game_state_transition(game, GAME_STATE_LOAD_MAP);
}

//  ---------------------------------------------------------------------------
void update_target_canidates(
    struct Actor* source,
    struct Zone* zone,
    struct World* world)
{
    struct ActorList* actors = zone->actors;

    target_canidate_count = 0;

    struct ActorListNode* actor_node = actors->front;
    while (actor_node != NULL)
    {
        struct Actor* actor = actor_node->actor;

        if (actor != source &&
            is_actor_alive(actor) &&
            is_actor_foe(source, actor) &&
            target_canidate_count < MAX_TARGET_CANIDATES)
        {
            if (can_see_actor(zone->map, source, actor, actors))
            {
                target_canidates[target_canidate_count++] = actor;
            }
        }

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
void update_target(
    struct Actor* source,
    struct Zone* zone,
    struct World* world)
{
    //  How long before a hostile actor changes its target
    const int HOSTILE_TARGET_TURNS = 3;

    if (source->ai->target != NULL)
    {
        if (is_actor_dead(source->ai->target))
        {
            source->ai->target = NULL;
        }
        else if (source->ai->hostile)
        {
            if (source->ai->target_turn_count <= HOSTILE_TARGET_TURNS)
            {
                //  Hostile actor hasn't lost interest in target yet
                ++source->ai->target_turn_count;
                return;
            }
        }
        else
        {
            //  Non-hostile actor is keeping same target.
            ++source->ai->target_turn_count;
            return;
        }
    }

    //  Reset target and turn counter
    source->ai->target_turn_count = 0;
    source->ai->target = NULL;

    //  Non-hostile villains just target the player
    if (source->type == ACTOR_TYPE_VILLAIN &&
        source->ai->hostile == 0)
    {
        if (can_see_actor(zone->map, source, world->player.actor, zone->actors))
        {
            source->ai->target = world->player.actor;
        }
        return;
    }

    update_target_canidates(source, zone, world);

    if (target_canidate_count > 0)
    {
        //  Hostile actors choose a random visible target
        int r = rand() % target_canidate_count;
        source->ai->target = target_canidates[r];
    }
    else
    {
        source->ai->target = NULL;
    }
}

//  ---------------------------------------------------------------------------
void simulate_world(struct Game* game, struct World* world)
{
    static struct Map* last_map = NULL;
    static struct PathFinder* path_finder = NULL;

    //  Create pathfinder if needed
    if (last_map != world->zone->map && world != NULL)
    {
        last_map = world->zone->map;

        if (path_finder != NULL)
        {
            destroy_path_finder(&path_finder);
            path_finder = NULL;
        }

        path_finder = create_path_finder(world->zone->map);
    }

    if (active_actor == NULL)
    {
        active_actor = world->player.actor;
        reset_action_points(world);
    }

    if (active_actor != NULL)
    {
        update_vision(active_actor->map, active_actor, world->zone->actors);

        if (active_actor->type == ACTOR_TYPE_VILLAIN)
        {
            if (active_actor->ai->enabled)
            {
                update_target(
                    active_actor,
                    world->zone,
                    world);

                if (active_actor->ai->target != NULL &&
                    is_actor_alive(active_actor->ai->target))
                {
                    struct Path* path = build_path(
                        path_finder,
                        active_actor->tile,
                        active_actor->ai->target->tile,
                        world->zone->map,
                        world->zone->actors);

                    if (path == NULL)
                    {
                        active_actor->move_direction = get_random_direction();
                    }
                    else
                    {
                        struct Point* next_point =
                            get_next_path_point(path, active_actor->tile);

                        if (next_point != NULL)
                        {
                            active_actor->move_direction = get_direction_by_delta(
                                active_actor->tile->x,
                                active_actor->tile->y,
                                next_point->x,
                                next_point->y);
                        }

                        destroy_path(&path);
                    }
                }
            }

        }

        if (active_actor->action_points > 0)
        {
            move_actor(active_actor, game, world);

            if (active_actor != NULL &&
                active_actor->type == ACTOR_TYPE_VILLAIN &&
                active_actor->action_points > 0)
            {
                //  Villains always expend action points to prevent
                //  their turn from never ending
                --active_actor->action_points;
            }
        }

        //  Activate next actor here to prevent processing an actor more
        //  times than needed. Check if NULL since move_actor can set
        //  active_actor to NULL when using a map link.
        if (active_actor != NULL &&
            active_actor->action_points <= 0)
        {
            next_active_actor(world);
        }
    }

    if (is_actor_dead(world->player.actor))
    {
        active_actor = NULL;
        begin_game_state_transition(game, GAME_STATE_GAME_OVER);
    }
}