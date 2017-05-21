#include "world.h"
#include "actor.h"
#include "actor_list.h"
#include "audio.h"
#include "map.h"
#include "tile.h"
#include "game_state_transition.h"
#include <assert.h>
#include <stdlib.h>

static const int BonesTilesetId = 95;

//  ---------------------------------------------------------------------------
void Attack(struct Actor* source, struct Actor* target)
{
    if (source->Health > 0 && target->Health > 0)
    {
        PlaySfx(SFX_ATTACK_01);

        target->Health -= 50;

        if (target->Health <= 0)
        {
            target->Collision = 0;
            target->Health = 0;
            target->MoveDirection = DIRECTION_NONE;
            target->TilesetId = BonesTilesetId;
        }
    }
}

//  ---------------------------------------------------------------------------
struct Actor* CreatePlayerActor(struct World* world)
{
    assert(world->Map != NULL);
    assert(world->Player.Actor == NULL);

    struct Actor* actor = CreateActor(world->Map, "Player", 12, 10, 190);
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
void MoveActors(struct Game* game, struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

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
                            IsFoe(actor, otherActor))
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
                Attack(actor, target);
            }
            //  Move actor to destination tile
            else if (canMove)
            {
                if (destTile->Link == NULL)
                {
                    actor->Tile = destTile;
                }
                else
                {
                    //  Map links are currently usable by player only
                    if (actor == world->Player.Actor)
                    {
                        BeginTransition(game, destTile->Link, DIRECTION_NONE);
                        return;
                    }
                }
            }
        }

        actorNode = actorNode->Next;
    } 
}

//  ---------------------------------------------------------------------------
void SimulateWorld(struct Game* game, struct World* world)
{
    MoveActors(game, world);
}