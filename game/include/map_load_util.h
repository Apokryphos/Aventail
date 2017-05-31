#ifndef MAP_LOAD_UTIL_HEADER_INCLUDED
#define MAP_LOAD_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

struct Map* load_map_from_file(
    FILE* file,
    struct Map** map,
    struct ActorList** actors);

#endif