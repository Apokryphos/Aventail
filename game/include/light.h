#ifndef LIGHT_HEADER_INCLUDED
#define LIGHT_HEADER_INCLUDED

struct World;

void update_lighting(
    struct Map* map,
    struct Actor* actor,
    struct ActorList* actors);

#endif