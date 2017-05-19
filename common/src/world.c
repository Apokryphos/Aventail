#include "world.h"
#include "actor.h"
#include "actor_list.h"
#include "map.h"
#include "tile.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct Actor* CreatePlayerActor(struct World* world)
{
    assert(world->Map != NULL);
    assert(world->Player.Actor == NULL);

    struct Actor* actor = CreateActor(world->Map, 12, 10, 190);
    actor->Type = ACTOR_TYPE_PLAYER;
    AddActor(world->Actors, actor);
    world->Player.Actor = actor;

    return actor;
}

//  ---------------------------------------------------------------------------
struct World* CreateWorld()
{
    struct World* world = malloc(sizeof(struct World));
    world->Actors = CreateActorList();
    world->LoadMapLink = NULL;
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
void EnterMapLink(struct World* world, struct MapLink* link)
{
    world->LoadMapLink = link;  
}

//  ---------------------------------------------------------------------------
void MoveActors(struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

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
                        canMove = 0;
                    }

                    if (otherActor->OnTouch != NULL)
                    {
                        (*otherActor->OnTouch)(actor, otherActor);
                    }
                }
                otherActorNode = otherActorNode->Next;
            }
            
            //  Move actor to destination tile
            if (canMove)
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
                        EnterMapLink(world, destTile->Link);
                        return;
                    }
                }
            }
        }

        actorNode = actorNode->Next;
    } 
}

//  ---------------------------------------------------------------------------
void SimulateWorld(struct World* world)
{
    MoveActors(world);
}