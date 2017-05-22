#ifndef MAP_LOAD_UTIL_HEADER_INCLUDED
#define MAP_LOAD_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

void LoadActorsFromFile(FILE* file, struct Map* map, struct ActorList* actors);
struct Map* LoadMapFromFile(FILE* file);

#endif