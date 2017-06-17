#ifndef ACTOR_AI_HEADER_INCLUDED
#define ACTOR_AI_HEADER_INCLUDED

struct Actor;

struct ActorAi
{
    /*
    *   Specifies if AI is enabled.
    */
    int enabled;

    /*
    *   Used by hostile AI to distinguish between factions.
    */
    int faction;

    /*
    *   If non-zero, AI will attack any non-faction actor.
    *   Hostile actors will attack the player or villains.
    */
    int hostile;

    /*
    *   Number of turns the target has been the same.
    */
    int target_turn_count;

    /*
    *   Current target actor (e.g. for pursuit).
    */
    struct Actor* target;
};

struct ActorAi* create_actor_ai();
void destroy_actor_ai(struct ActorAi** actor_ai);

#endif