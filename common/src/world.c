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
    AddActor(world->Actors, actor);
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
void MoveActors(struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        struct Tile* destTile = GetNeighbor(actor->Map, actor->Tile, actor->MoveDirection);

        //  Check if destination tile is valid
        if (destTile != NULL && destTile->Collision == 0)
        {
            int canMove = 1;

            struct ActorListNode* otherActorNode = world->Actors->First;
            while (otherActorNode != NULL)
            {
                struct Actor* otherActor = otherActorNode->Actor;
                
                //  Check if another actor occupies destination tile
                if (otherActor->Tile == destTile && otherActor != actor)
                {
                    canMove = 0;
                    break;
                }
                otherActorNode = otherActorNode->Next;
            }
            
            //  Move actor to destination tile
            if (canMove)
            {
                actor->Tile = destTile;
                actor->MoveDirection = DIRECTION_NONE;
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