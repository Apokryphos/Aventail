#ifndef ACTOR_FUNCS_HEADER_INCLUDED
#define ACTOR_FUNCS_HEADER_INCLUDED

struct Actor;

void ActivateContainer(struct Actor* source, struct Actor* target);
void ActivateDoor(struct Actor* source, struct Actor* target);

#endif