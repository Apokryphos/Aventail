#ifndef VISION_HEADER_INCLUDED
#define VISION_HEADER_INCLUDED

struct Actor;
struct ActorList;
struct Map;
struct VisionMap;

int can_see_actor(
    const struct Map* map,
    const struct Actor* source,
    const struct Actor* target,
    const struct ActorList* actors);

void destroy_vision_map(struct VisionMap** vision_map);

void update_vision(
    const struct Map* map,
    struct Actor* source,
    const struct ActorList* actors);

#endif