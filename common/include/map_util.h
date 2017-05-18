#ifndef MAP_UTIL_HEADER_INCLUDED
#define MAP_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

void LoadActorsFromFile(FILE* file, struct Map* map, struct ActorList* actors);
struct Map* LoadMapFromFile(FILE* file);
void SaveActorsToFile(FILE* file, const struct ActorList* actors);
void SaveMapToFile(FILE* file, const struct Map* map);

#endif