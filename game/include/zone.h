#ifndef ZONE_HEADER_INCLUDED
#define ZONE_HEADER_INCLUDED

struct ActorList;
struct Map;

struct Zone
{
    struct ActorList* actors;
    struct Map* map;
};

struct Zone* create_zone();
void destroy_zone(struct Zone** zone);

#endif