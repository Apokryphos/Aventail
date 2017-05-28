#ifndef MAP_SAVE_UTIL_HEADER_INCLUDED
#define MAP_SAVE_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

void save_actors_to_file(FILE* file, const struct ActorList* actors);
void save_map_to_file(FILE* file, const struct Map* map);

#endif