#include "actor.h"
#include <string.h>

//  ---------------------------------------------------------------------------
void LoadActorDefinition(struct Actor* actor)
{
    //  Simple definitions until loading from data files is supported
    if (strcasecmp(actor->Name, "slime") == 0)
    {
        actor->MaxHealth = 6;
        actor->Health = actor->MaxHealth;
        actor->Stats.Attack = 3;
        actor->Stats.Defend = 1;
    }
    else if (strcasecmp(actor->Name, "spider") == 0)
    {
        actor->MaxHealth = 8;
        actor->Health = actor->MaxHealth;
        actor->Stats.Attack = 3;
        actor->Stats.Defend = 4;
    }
}

//  ---------------------------------------------------------------------------
void LoadPlayerDefinition(struct Actor* actor)
{
    actor->MaxHealth = 20;
    actor->Health = actor->MaxHealth;
    actor->Stats.Attack = 2;
    actor->Stats.Defend = 2;
}