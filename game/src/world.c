#include "world.h"
#include "actor.h"
#include "actor_list.h"
#include "audio.h"
#include "inventory.h"
#include "map.h"
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
    assert(source->ActionPoints > 0);

    if (source->Health > 0 && target->Health > 0)
    {
        PlaySfx(SFX_ATTACK_01);

        --source->ActionPoints;

        target->Health -= 50;

        if (target->Health <= 0)
        {
            target->Collision = 0;
            target->Health = 0;
            target->MoveDirection = DIRECTION_NONE;
            target->TilesetId = BonesTilesetId;

            LootActor(source, target, world);
        }
    }
}

//  ---------------------------------------------------------------------------
static int CanAct(const struct Actor* actor)
{
    return 
        (actor->Type == ACTOR_TYPE_PLAYER || 
        actor->Type == ACTOR_TYPE_VILLAIN) &&
        actor->Health > 0 &&
        actor->ActionPoints > 0;
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

    struct Actor* actor = CreateActor(world->Map, "Loot", x, y, 91);
    actor->Type = ACTOR_TYPE_CONTAINER;
    AddActorToFront(world->Actors, actor);

    GiveInventoryItems(inventory, actor->Inventory);

    return actor;
}

//  ---------------------------------------------------------------------------
struct Actor* CreatePlayerActor(struct World* world)
{
    assert(world->Player.Actor == NULL);

    struct Actor* actor = CreateActor(NULL, "Player", -1, -1, 190);
    actor->Type = ACTOR_TYPE_PLAYER;
    AddActorToFront(world->Actors, actor);
    world->Player.Actor = actor;

    return actor;
}

//  ---------------------------------------------------------------------------
struct World* CreateWorld()
{
    struct World* world = malloc(sizeof(struct World));
    world->Actors = CreateActorList();
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

    DestroyActorList(&(*world)->Actors);

    if ((*world)->Map != NULL)
    {
        DestroyMap(&(*world)->Map);
    }

    free(*world);
    *world = NULL;
}

//  ---------------------------------------------------------------------------
static void LootActor(struct Actor* source, struct Actor* target, struct World* world)
{
    if (target->Cash > 0)
    {
        source->Cash += target->Cash;
        target->Cash = 0;
    }

    if (GetInventoryItemCount(target->Inventory) > 0)
    {
        GiveInventoryItems(target->Inventory, source->Inventory);

        if (GetInventoryItemCount(target->Inventory) > 0)
        {
            CreateLootContainer(world, target->Tile->X, target->Tile->Y, target->Inventory);
        }
    }   
}

//  ---------------------------------------------------------------------------
static void MoveActor(struct Actor* actor, struct Game* game, struct World* world)
{
    assert(actor->ActionPoints > 0);

    if (actor->MoveDirection == DIRECTION_NONE)
    {
        return;
    }

    struct Actor* target = NULL;

    struct Tile* destTile = GetNeighbor(actor->Map, actor->Tile, actor->MoveDirection);

    //  Reset move direction
    actor->MoveDirection = DIRECTION_NONE;

    //  Check if destination tile is valid
    if (destTile != NULL && destTile->Collision == 0)
    {
        int canMove = 1;

        struct ActorListNode* otherActorNode = world->Actors->First;
        while (otherActorNode != NULL)
        {
            struct Actor* otherActor = otherActorNode->Actor;

            //  Check if another actor occupies destination tile
            //  and is collidable
            if (otherActor->Tile == destTile &&
                otherActor != actor)
            {
                if (otherActor->Collision)
                {
                    //  Attack first foe on tile
                    if (target == NULL &&
                        ActorIsFoe(actor, otherActor))
                    {
                        target = otherActor;
                    }

                    canMove = 0;
                }

                if (otherActor->OnTouch != NULL)
                {
                    (*otherActor->OnTouch)(actor, otherActor);
                }
            }
            otherActorNode = otherActorNode->Next;
        }

        //  Attack target if any
        if (target != NULL)
        {
            Attack(actor, target, world);
        }
        //  Move actor to destination tile
        else if (canMove)
        {
            --actor->ActionPoints;

            if (destTile->Link == NULL)
            {
                actor->Tile = destTile;
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

    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        if (CanAct(actor))
        {
            ActiveActor = actor;
            return;
        }

        actorNode = actorNode->Next;
    }
}

//  ---------------------------------------------------------------------------
static void ResetActionPoints(struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        actor->ActionPoints = actor->MaxActionPoints;

        actorNode = actorNode->Next;
    }
}

//  ---------------------------------------------------------------------------
void SimulateWorld(struct Game* game, struct World* world)
{
    if (ActiveActor == NULL)
    {
        ActiveActor = world->Player.Actor;
        ResetActionPoints(world);
    }

    if (ActiveActor != NULL)
    {
        if (ActiveActor->Type == ACTOR_TYPE_VILLAIN)
        {
            ActiveActor->MoveDirection = GetRandomDirection();
        }

        if (ActiveActor->ActionPoints > 0)
        {
            MoveActor(ActiveActor, game, world);

            if (ActiveActor != NULL &&
                ActiveActor->Type == ACTOR_TYPE_VILLAIN &&
                ActiveActor->ActionPoints > 0)
            {
                //  Villains always expend action points to prevent
                //  their turn from never ending
                --ActiveActor->ActionPoints;
            }
        }
        else
        {
            NextActiveActor(world);
        }
    }

    if (ActorIsDead(world->Player.Actor))
    {
        ActiveActor = NULL;
        BeginGameStateTransition(game, GAME_STATE_GAME_OVER);
    }
}