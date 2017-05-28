#include "map.h"
#include "map_link.h"
#include "tile.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Map* create_map(
    const int width,
    const int height,
    const int tile_width,
    const int tile_height)
{
    assert(width > 0);
    assert(height > 0);
    assert(tile_width > 0);
    assert(tile_height > 0);

    struct Map* map = malloc(sizeof(struct Map));
    map->width = width;
    map->height = height;
    map->tile_width = tile_width;
    map->tile_height = tile_height;

    map->tiles = malloc(sizeof(struct Tile) * width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            struct Tile* tile = &map->tiles[y * width + x];
            tile->Collision = 0;
            tile->TilesetId = 0;
            tile->X = x;
            tile->Y = y;
            tile->Link = NULL;
        }
    }

    return map;
}

//  ---------------------------------------------------------------------------
void destroy_map(struct Map** map)
{
    if (*map == NULL)
    {
        return;
    }

    assert((*map)->tiles != NULL);
    int tileCount = (*map)->width * (*map)->height;
    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &(*map)->tiles[t];
        if (tile->Link != NULL)
        {
            //  Free tile links
            destroy_map_link(&tile->Link);
        }
    }
    free((*map)->tiles);

    free(*map);
    *map = NULL;
}

//  ---------------------------------------------------------------------------
struct Tile* get_tile_neighbor(
    const struct Map* map,
    const struct Tile* tile,
    const enum Direction direction)
{
    assert(map != NULL);
    assert(tile != NULL);

    int dx, dy;
    get_direction_delta(direction, &dx, &dy);
    return get_map_tile(map, tile->X + dx, tile->Y + dy);
}

//  ---------------------------------------------------------------------------
struct Tile* get_map_tile(const struct Map* map, const int x, const int y)
{
    assert(map != NULL);
    return in_map_bounds(map, x, y) ? &map->tiles[y * map->width + x] : NULL;
}

//  ---------------------------------------------------------------------------
int get_map_tile_count(const struct Map* map)
{
    assert(map != NULL);
    return map->width * map->height;
}

//  ---------------------------------------------------------------------------
size_t get_map_tile_index(const struct Map* map, struct Tile* tile)
{
    assert(map != NULL);
    assert(tile != NULL);
    return tile->Y * map->width + tile->X;
}

//  ---------------------------------------------------------------------------
int in_map_bounds(const struct Map* map, const int x, const int y)
{
    assert(map != NULL);
    return x >= 0 && y >= 0 && x < map->width && y < map->height;
}