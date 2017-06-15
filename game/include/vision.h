#ifndef VISION_HEADER_INCLUDED
#define VISION_HEADER_INCLUDED

struct Actor;
struct ActorList;
struct Map;

int can_see_actor(
    const struct Map* map,
    const struct Actor* source,
    const struct Actor* target,
    const struct ActorList* actors);

#endif