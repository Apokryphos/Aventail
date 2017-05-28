#include "world.h"
#include "actor.h"
#include "actor_defs.h"
#include "actor_list.h"
#include "a_star.h"
#include "audio.h"
#include "gear.h"
#include "inventory.h"
#include "item_defs.h"
#include "map.h"
#include "point.h"
#include "tile.h"
#include "game_state_transition.h"
#include <assert.h>
#include <stdlib.h>

static struct Actor* ActiveActor = NULL;
static const int BonesTilesetId = 95;

static void LootActor(struct Actor* source, struct Actor* target, struct World* world);

//  ---------------------------------------------------------------------------
static void Attack(struct Actor* source, struct Actor* target, struct World* world)
{
    assert(source->action_points > 0);

    if (source->health > 0 && target->health > 0)
    {
        PlaySfx(SFX_ATTACK_01);

        --source->action_points;

        struct Stats sourceStats = AddStats(source->stats, source->gear.stats);
        struct Stats targetStats = AddStats(target->stats, target->gear.stats);

        int damage = 1;
        if (sourceStats.Attack > targetStats.Defend)
        {
            damage = (sourceStats.Attack - targetStats.Defend) + 1;
        }

        target->health -= damage;

        if (target->health <= 0)
        {
            target->collision = 0;
            target->health = 0;
            target->move_direction = DIRECTION_NONE;
            target->tileset_id = BonesTilesetId;

            LootActor(source, target, world);
        }
    }
}

//  ---------------------------------------------------------------------------
static int CanAct(const struct Actor* actor)
{
    return
        (actor->type == ACTOR_TYPE_PLAYER ||
        actor->type == ACTOR_TYPE_VILLAIN) &&
        actor->health > 0 &&
        actor->action_points > 0;
}

//  ---------------------------------------------------------------------------
static struct Actor* CreateLootContainer(
    struct World* world,
    const int x,
    const int y,
    struct Inventory* inventory)
{
    assert(world->Map != NULL);
    assert(inventory != NULL);

    struct Actor* actor = create_actor(world->Map, "Loot", x, y, 91);
    actor->type = ACTOR_TYPE_CONTAINER;
    add_actor_to_actor_list_front(world->Actors, actor);

    move_inventory_items(inventory, actor->inventory);

    return actor;
}

//  ---------------------------------------------------------------------------
struct Actor* CreatePlayerActor(struct World* world)
{
    assert(world->Player.Actor == NULL);

    struct Actor* actor = create_actor(NULL, "Player", -1, -1, 190);
    actor->type = ACTOR_TYPE_PLAYER;
    LoadPlayerDefinition(actor);
    add_actor_to_actor_list_front(world->Actors, actor);
    world->Player.Actor = actor;

    //  Some starting inventory to test with
    struct Item* armor = create_item("Leather Cuirass");
    LoadItemDefinition(armor);
    equip_item(actor, armor);

    // struct Item* weapon = create_item("Bronze Sword");
    // LoadItemDefinition(weapon);
    // equip_item(actor, weapon);

    struct Item* armor2 = create_item("Chainmail");
    LoadItemDefinition(armor2);
    add_item_to_inventory(actor->inventory, armor2);

    // struct Item* shield1 = create_item("Buckler");
    // LoadItemDefinition(shield1);
    // add_item_to_inventory(actor->inventory, shield1);

    // add_item_to_inventory(actor->inventory, CreateWeapon("Dagger"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Dirk"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Knife"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Shiv"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Katana"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Nodachi"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Short Sword"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Great Sword"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Claymore"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Longsword"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Broadsword"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Sabre"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Rapier"));
    // add_item_to_inventory(actor->inventory, CreateWeapon("Cutlass"));

    // struct Item* weapon2 = create_item("Iron Short Sword");
    // LoadItemDefinition(weapon2);
    // add_item_to_inventory(actor->inventory, weapon2);

    struct Item* accessory1 = create_item("Healing Ring");
    LoadItemDefinition(accessory1);
    add_item_to_inventory(actor->inventory, accessory1);

    return actor;
}

//  ---------------------------------------------------------------------------
struct World* CreateWorld()
{
    struct World* world = malloc(sizeof(struct World));
    world->Actors = create_actor_list();
    world->Player.Actor = NULL;
    world->Map = NULL;
    return world;
}

//  ---------------------------------------------------------------------------
void DestroyWorld(struct World** world)
{
    if (*world == NULL)
    {
        return;
    }

    destroy_actor_list(&(*world)->Actors);

    if ((*world)->Map != NULL)
    {
        destroy_map(&(*world)->Map);
    }

    free(*world);
    *world = NULL;
}

//  ---------------------------------------------------------------------------
static void LootActor(struct Actor* source, struct Actor* target, struct World* world)
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
            CreateLootContainer(world, target->tile->X, target->tile->Y, target->inventory);
        }
    }
}

//  ---------------------------------------------------------------------------
static void MoveActor(struct Actor* actor, struct Game* game, struct World* world)
{
    assert(actor->action_points > 0);

    if (actor->move_direction == DIRECTION_NONE)
    {
        return;
    }

    struct Actor* target = NULL;

    struct Tile* destTile = get_tile_neighbor(
        actor->map,
        actor->tile,
        actor->move_direction);

    //  Reset move direction
    actor->move_direction = DIRECTION_NONE;

    //  Check if destination tile is valid
    if (destTile != NULL && destTile->Collision == 0)
    {
        int canMove = 1;

        struct ActorListNode* otherActorNode = world->Actors->front;
        while (otherActorNode != NULL)
        {
            struct Actor* otherActor = otherActorNode->actor;

            //  Check if another actor occupies destination tile
            //  and is collidable
            if (otherActor->tile == destTile &&
                otherActor != actor)
            {
                if (otherActor->collision)
                {
                    //  Attack first foe on tile
                    if (target == NULL &&
                        is_actor_foe(actor, otherActor))
                    {
                        target = otherActor;
                    }

                    canMove = 0;
                }

                if (otherActor->on_touch != NULL)
                {
                    (*otherActor->on_touch)(actor, otherActor);
                }
            }
            otherActorNode = otherActorNode->next;
        }

        //  Attack target if any
        if (target != NULL)
        {
            Attack(actor, target, world);
        }
        //  Move actor to destination tile
        else if (canMove)
        {
            --actor->action_points;

            if (destTile->Link == NULL)
            {
                actor->tile = destTile;
            }
            else
            {
                //  Map links are currently usable by player only
                if (actor == world->Player.Actor)
                {
                    ActiveActor = NULL;
                    BeginMapLinkTransition(game, destTile->Link, DIRECTION_NONE);
                    return;
                }
            }
        }
    }
}

//  ---------------------------------------------------------------------------
static void NextActiveActor(struct World* world)
{
    ActiveActor = NULL;

    struct ActorListNode* actorNode = world->Actors->front;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->actor;

        if (CanAct(actor))
        {
            ActiveActor = actor;
            return;
        }

        actorNode = actorNode->next;
    }
}

//  ---------------------------------------------------------------------------
static void ResetActionPoints(struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->front;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->actor;

        actor->action_points = actor->max_action_points;

        actorNode = actorNode->next;
    }
}

//  ---------------------------------------------------------------------------
void SimulateWorld(struct Game* game, struct World* world)
{
    static struct Map* lastMap = NULL;
    static struct AStar* aStar = NULL;

    if (lastMap != world->Map && world != NULL)
    {
        lastMap = world->Map;

        if (aStar != NULL)
        {
            DestroyAStar(&aStar);
            aStar = NULL;
        }

        aStar = CreateAStar(world->Map);
    }

    if (ActiveActor == NULL)
    {
        ActiveActor = world->Player.Actor;
        ResetActionPoints(world);
    }

    if (ActiveActor != NULL)
    {
        if (ActiveActor->type == ACTOR_TYPE_VILLAIN)
        {
            struct AStarPath* path = BuildAStarPath(
                aStar,
                ActiveActor->tile,
                world->Player.Actor->tile,
                world->Map,
                world->Actors);

            if (path == NULL)
            {
                ActiveActor->move_direction = get_random_direction();
            }
            else
            {
                struct Point* nextPoint =
                    GetNextPathPoint(path, ActiveActor->tile);

                if (nextPoint != NULL)
                {
                    ActiveActor->move_direction = get_direction_by_delta(
                        ActiveActor->tile->X,
                        ActiveActor->tile->Y,
                        nextPoint->X,
                        nextPoint->Y);
                }

                DestroyAStarPath(&path);
            }
        }

        if (ActiveActor->action_points > 0)
        {
            MoveActor(ActiveActor, game, world);

            if (ActiveActor != NULL &&
                ActiveActor->type == ACTOR_TYPE_VILLAIN &&
                ActiveActor->action_points > 0)
            {
                //  Villains always expend action points to prevent
                //  their turn from never ending
                --ActiveActor->action_points;
            }
        }
        else
        {
            NextActiveActor(world);
        }
    }

    if (is_actor_dead(world->Player.Actor))
    {
        ActiveActor = NULL;
        BeginGameStateTransition(game, GAME_STATE_GAME_OVER);
    }
}