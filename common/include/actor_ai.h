#ifndef ACTOR_AI_HEADER_INCLUDED
#define ACTOR_AI_HEADER_INCLUDED

struct Actor;

struct ActorAi
{
    struct Actor* target;
};

struct ActorAi* create_actor_ai();
void destroy_actor_ai(struct ActorAi** actor_ai);
int can_see_actor(
    struct World* world,
    const struct Actor* source,
    const struct Actor* target);

#endif