#include "actor.h"
#include <string.h>

//  ---------------------------------------------------------------------------
void load_actor_definition(struct Actor* actor)
{
    //  Simple definitions until loading from data files is supported
    if (strcasecmp(actor->name, "slime") == 0)
    {
        actor->max_health = 6;
        actor->health = actor->max_health;
        actor->stats.attack = 3;
        actor->stats.defend = 1;
    }
    else if (strcasecmp(actor->name, "spider") == 0)
    {
        actor->max_health = 8;
        actor->health = actor->max_health;
        actor->stats.attack = 3;
        actor->stats.defend = 4;
    }
}

//  ---------------------------------------------------------------------------
void load_player_definition(struct Actor* actor)
{
    actor->max_health = 20;
    actor->health = actor->max_health;
    actor->stats.attack = 2;
    actor->stats.defend = 2;
}