#ifndef VISION_HEADER_INCLUDED
#define VISION_HEADER_INCLUDED

struct Actor;
struct World;

int can_see_actor(
    const struct World* world,
    const struct Actor* source,
    const struct Actor* target);

#endif