#include "map.h"
#include "map_link.h"
#include "tile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_MAP_SIZE = 25;

//  ---------------------------------------------------------------------------
struct Map* create_map(
    const int width,
    const int height,
    const int tile_width,
    const int tile_height)
{
    if (width <= 0 || width > MAX_MAP_SIZE)
    {
        fprintf(
            stderr,
            "Invalid map width specified. Map width must be greater than zero and less than or equal to %d.\n",
            MAX_MAP_SIZE);
        fprintf(
            stderr,
            "Specified map width is %d.\n",
            width);
        return NULL;
    }

    if (height <= 0 || height > MAX_MAP_SIZE)
    {
        fprintf(
            stderr,
            "Invalid map height specified. Map height must be greater than zero and less than or equal to %d.\n",
            MAX_MAP_SIZE);
        fprintf(
            stderr,
            "Specified map height is %d.\n",
            height);
        return NULL;
    }

    struct Map* map = malloc(sizeof(struct Map));
    if (map == NULL)
    {
        perror("");
        fprintf(stderr, "Failed to allocate map.\n");
        free(map);
        return NULL;
    }

    map->width = width;
    map->height = height;
    map->tile_width = tile_width;
    map->tile_height = tile_height;
    map->sunlight = 0;

    map->tiles = malloc(sizeof(struct Tile) * width * height);
    if (map->tiles == NULL)
    {
        perror("");
        fprintf(stderr, "Failed to allocate map tiles.\n");
        free(map);
        return NULL;
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            struct Tile* tile = &map->tiles[y * width + x];
            tile->collision = 0;
            tile->light = 0;
            tile->tileset_id = 0;
            tile->x = x;
            tile->y = y;
            tile->link = NULL;
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
        if (tile->link != NULL)
        {
            //  Free tile links
            destroy_map_link(&tile->link);
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
    return get_map_tile(map, tile->x + dx, tile->y + dy);
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
    return tile->y * map->width + tile->x;
}

//  ---------------------------------------------------------------------------
int in_map_bounds(const struct Map* map, const int x, const int y)
{
    assert(map != NULL);
    return x >= 0 && y >= 0 && x < map->width && y < map->height;
}

//  ---------------------------------------------------------------------------
int index_in_map_bounds(const struct Map* map, const int index)
{
    assert(map != NULL);
    return index >= 0 && index < map->width * map->height;
}