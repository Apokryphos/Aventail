#ifndef MAP_HEADER_INCLUDED
#define MAP_HEADER_INCLUDED

#include "direction.h"
#include <stddef.h>

extern const int MAX_MAP_SIZE;

struct Tile;

struct Map
{
    int width;
    int height;
    int tile_width;
    int tile_height;
    int sunlight;
    struct Tile* tiles;
};

struct Map* create_map(
    const int width,
    const int height,
    const int tile_width,
    const int tile_height);
void destroy_map(struct Map** map);
struct Tile* get_tile_neighbor(
    const struct Map* map,
    const struct Tile* tile,
    const enum Direction direction);
struct Tile* get_map_tile(const struct Map* map, const int x, const int y);
int get_map_tile_count(const struct Map* map);
size_t get_map_tile_index(const struct Map* map, struct Tile* tile);
int in_map_bounds(const struct Map* map, const int x, const int y);
int index_in_map_bounds(const struct Map* map, const int index);

#endif