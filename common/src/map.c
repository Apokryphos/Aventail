#include "map.h"
#include "tile.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct Map* CreateMap(
    const int width, 
    const int height, 
    const int tileWidth, 
    const int tileHeight)
{
    assert(width > 0);
    assert(height > 0);
    assert(tileWidth > 0);
    assert(tileHeight > 0);

    struct Map* map = malloc(sizeof(struct Map));
    map->Width = width;
    map->Height = height;
    map->TileWidth = tileWidth;
    map->TileHeight = tileHeight;

    map->Tiles = malloc(sizeof(struct Tile) * width * height);

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            struct Tile* tile = &map->Tiles[y * width + x];
            tile->Collision = 0;
            tile->TilesetId = 0;
            tile->X = x;
            tile->Y = y;
        }
    }

    return map;
}

//  ---------------------------------------------------------------------------
struct Tile* GetNeighbor(struct Map* map, struct Tile* tile, enum Direction direction)
{
    assert(map != NULL);
    assert(tile != NULL);

    int dx, dy;
    GetDelta(direction, &dx, &dy);
    return GetTile(map, tile->X + dx, tile->Y + dy);
}

//  ---------------------------------------------------------------------------
struct Tile* GetTile(const struct Map* map, const int x, const int y)
{
    assert(map != NULL);
    return InBounds(map, x, y) ? &map->Tiles[y * map->Width + x] : NULL;
}

//  ---------------------------------------------------------------------------
int GetTileCount(const struct Map* map)
{
    assert(map != NULL);
    return map->Width * map->Height;
}

//  ---------------------------------------------------------------------------
int InBounds(const struct Map* map, const int x, const int y)
{
    assert(map != NULL);
    return x >= 0 && y >= 0 && x < map->Width && y < map->Height;
}