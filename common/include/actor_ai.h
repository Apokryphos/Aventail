#ifndef ACTOR_AI_HEADER_INCLUDED
#define ACTOR_AI_HEADER_INCLUDED

struct Actor;

struct ActorAi
{
    struct Actor* target;
};

struct ActorAi* create_actor_ai();
void destroy_actor_ai(struct ActorAi** actor_ai);

#endif