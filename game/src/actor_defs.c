#include "actor.h"
#include "actor_ai.h"
#include <assert.h>
#include <string.h>

static const int FACTION_VILLAIN = 1;
static const int FACTION_SLIME = 2;

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
        actor->ai->enabled = 1;
        actor->ai->faction = FACTION_SLIME;
        actor->ai->hostile = 1;
    }
    else if (strcasecmp(actor->name, "spider") == 0)
    {
        actor->max_health = 8;
        actor->health = actor->max_health;
        actor->stats.attack = 3;
        actor->stats.defend = 4;
        actor->ai->enabled = 1;
        actor->ai->faction = FACTION_VILLAIN;
    }
    else if (strcasecmp(actor->name, "bat") == 0)
    {
        actor->max_health = 3;
        actor->health = actor->max_health;
        actor->stats.attack = 2;
        actor->stats.defend = 2;
        actor->ai->enabled = 1;
        actor->ai->faction = FACTION_VILLAIN;
    }
    else if (strcasecmp(actor->name, "gremlin") == 0)
    {
        actor->max_health = 8;
        actor->health = actor->max_health;
        actor->stats.attack = 4;
        actor->stats.defend = 4;
        actor->ai->enabled = 1;
        actor->ai->faction = FACTION_VILLAIN;
    }
    if (strcasecmp(actor->name, "giant slime") == 0)
    {
        actor->max_health = 10;
        actor->health = actor->max_health;
        actor->stats.attack = 4;
        actor->stats.defend = 4;
        actor->ai->enabled = 1;
        actor->ai->faction = FACTION_SLIME;
        actor->ai->hostile = 1;
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