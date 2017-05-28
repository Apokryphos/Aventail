#ifndef ACTOR_FUNCS_HEADER_INCLUDED
#define ACTOR_FUNCS_HEADER_INCLUDED

struct Actor;

void activate_container(struct Actor* source, struct Actor* target);
void activate_door(struct Actor* source, struct Actor* target);

#endif