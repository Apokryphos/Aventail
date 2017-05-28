#include "actor.h"
#include <string.h>

//  ---------------------------------------------------------------------------
void LoadActorDefinition(struct Actor* actor)
{
    //  Simple definitions until loading from data files is supported
    if (strcasecmp(actor->name, "slime") == 0)
    {
        actor->max_health = 6;
        actor->health = actor->max_health;
        actor->stats.Attack = 3;
        actor->stats.Defend = 1;
    }
    else if (strcasecmp(actor->name, "spider") == 0)
    {
        actor->max_health = 8;
        actor->health = actor->max_health;
        actor->stats.Attack = 3;
        actor->stats.Defend = 4;
    }
}

//  ---------------------------------------------------------------------------
void LoadPlayerDefinition(struct Actor* actor)
{
    actor->max_health = 20;
    actor->health = actor->max_health;
    actor->stats.Attack = 2;
    actor->stats.Defend = 2;
}