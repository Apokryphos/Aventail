#ifndef MAP_LOAD_UTIL_HEADER_INCLUDED
#define MAP_LOAD_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

void load_actors_from_file(FILE* file, struct Map* map, struct ActorList* actors);
struct Map* load_map_from_file(FILE* file);

#endif