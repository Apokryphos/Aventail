#include "world.h"
#include "actor.h"
#include "actor_defs.h"
#include "actor_list.h"
#include "path_finder.h"
#include "audio.h"
#include "game_state_transition.h"
#include "gear.h"
#include "inventory.h"
#include "item_defs.h"
#include "map.h"
#include "point.h"
#include "tile.h"
#include <assert.h>
#include <stdlib.h>

static struct Actor* active_actor = NULL;
static const int BONES_TILESET_ID = 95;

static void loot_actor(
    struct Actor* source,
    struct Actor* target,
    struct World* world);

//  ---------------------------------------------------------------------------
static void attack_actor(
    struct Actor* source,
    struct Actor* target,
    struct World* world)
{
    assert(source->action_points > 0);

    if (source->health > 0 && target->health > 0)
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

        if (target->health <= 0)
        {
            target->collision = 0;
            target->health = 0;
            target->move_direction = DIRECTION_NONE;
            target->tileset_id = BONES_TILESET_ID;

            loot_actor(source, target, world);
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
static struct Actor* create_loot_container(
    struct World* world,
    const int x,
    const int y,
    struct Inventory* inventory)
{
    assert(world->map != NULL);
    assert(inventory != NULL);

    struct Actor* actor = create_actor(world->map, "Loot", x, y, 91);
    actor->type = ACTOR_TYPE_CONTAINER;
    add_actor_to_actor_list_front(world->actors, actor);

    move_inventory_items(inventory, actor->inventory);

    return actor;
}

//  ---------------------------------------------------------------------------
struct Actor* create_player_actor(struct World* world)
{
    assert(world->player.actor == NULL);

    struct Actor* actor = create_actor(NULL, "Player", -1, -1, 190);
    actor->type = ACTOR_TYPE_PLAYER;
    load_player_definition(actor);
    add_actor_to_actor_list_front(world->actors, actor);
    world->player.actor = actor;

    //  Some starting inventory to test with
    struct Item* armor = create_item("Leather Cuirass");
    load_item_definition(armor);
    equip_item(actor, armor);

    // struct Item* weapon = create_item("Bronze Sword");
    // load_item_definition(weapon);
    // equip_item(actor, weapon);

    struct Item* armor2 = create_item("Chainmail");
    load_item_definition(armor2);
    add_item_to_inventory(actor->inventory, armor2);

    // struct Item* shield1 = create_item("Buckler");
    // load_item_definition(shield1);
    // add_item_to_inventory(actor->inventory, shield1);

    // add_item_to_inventory(actor->inventory, create_weapon("Dagger"));
    // add_item_to_inventory(actor->inventory, create_weapon("Dirk"));
    // add_item_to_inventory(actor->inventory, create_weapon("Knife"));
    // add_item_to_inventory(actor->inventory, create_weapon("Shiv"));
    // add_item_to_inventory(actor->inventory, create_weapon("Katana"));
    // add_item_to_inventory(actor->inventory, create_weapon("Nodachi"));
    // add_item_to_inventory(actor->inventory, create_weapon("Short Sword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Great Sword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Claymore"));
    // add_item_to_inventory(actor->inventory, create_weapon("Longsword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Broadsword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Sabre"));
    // add_item_to_inventory(actor->inventory, create_weapon("Rapier"));
    // add_item_to_inventory(actor->inventory, create_weapon("Cutlass"));

    // struct Item* weapon2 = create_item("Iron Short Sword");
    // load_item_definition(weapon2);
    // add_item_to_inventory(actor->inventory, weapon2);

    struct Item* accessory1 = create_item("Healing Ring");
    load_item_definition(accessory1);
    add_item_to_inventory(actor->inventory, accessory1);

    return actor;
}

//  ---------------------------------------------------------------------------
struct World* create_world()
{
    struct World* world = malloc(sizeof(struct World));
    world->actors = create_actor_list();
    world->player.actor = NULL;
    world->map = NULL;
    return world;
}

//  ---------------------------------------------------------------------------
void destroy_world(struct World** world)
{
    if (*world == NULL)
    {
        return;
    }

    destroy_actor_list(&(*world)->actors);

    if ((*world)->map != NULL)
    {
        destroy_map(&(*world)->map);
    }

    free(*world);
    *world = NULL;
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
static void MoveActor(
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

        struct ActorListNode* other_actor_node = world->actors->front;
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
                    begin_map_link_transition(game, dest_tile->link, DIRECTION_NONE);
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

    struct ActorListNode* actor_node = world->actors->front;
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
    struct ActorListNode* actor_node = world->actors->front;
    while (actor_node != NULL)
    {
        struct Actor* actor = actor_node->actor;

        actor->action_points = actor->max_action_points;

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
void simulate_world(struct Game* game, struct World* world)
{
    static struct Map* last_map = NULL;
    static struct PathFinder* path_finder = NULL;

    if (last_map != world->map && world != NULL)
    {
        last_map = world->map;

        if (path_finder != NULL)
        {
            destroy_path_finder(&path_finder);
            path_finder = NULL;
        }

        path_finder = create_path_finder(world->map);
    }

    if (active_actor == NULL)
    {
        active_actor = world->player.actor;
        reset_action_points(world);
    }

    if (active_actor != NULL)
    {
        if (active_actor->type == ACTOR_TYPE_VILLAIN)
        {
            struct Path* path = build_path(
                path_finder,
                active_actor->tile,
                world->player.actor->tile,
                world->map,
                world->actors);

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

        if (active_actor->action_points > 0)
        {
            MoveActor(active_actor, game, world);

            if (active_actor != NULL &&
                active_actor->type == ACTOR_TYPE_VILLAIN &&
                active_actor->action_points > 0)
            {
                //  Villains always expend action points to prevent
                //  their turn from never ending
                --active_actor->action_points;
            }
        }
        else
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