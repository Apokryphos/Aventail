#ifndef MAP_SAVE_UTIL_HEADER_INCLUDED
#define MAP_SAVE_UTIL_HEADER_INCLUDED

struct ActorList;
struct Map;

#include <stdio.h>

void SaveActorsToFile(FILE* file, const struct ActorList* actors);
void SaveMapToFile(FILE* file, const struct Map* map);

#endif