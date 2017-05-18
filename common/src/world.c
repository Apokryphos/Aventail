#include "world.h"
#include "actor.h"
#include "actor_list.h"
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
        free((*world)->Map);
    }

    free(*world);
    *world = NULL;
}

//  ---------------------------------------------------------------------------
void SimulateWorld(struct World* world)
{
    struct ActorListNode* actorNode = world->Actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;
        MoveActor(actor);

        actorNode = actorNode->Next;
    }    
}