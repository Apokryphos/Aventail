#ifndef MAP_HEADER_INCLUDED
#define MAP_HEADER_INCLUDED

#include "direction.h"
#include <stddef.h>

struct Tile;

struct Map
{
    int Width;
    int Height;
    int TileWidth;
    int TileHeight;
    struct Tile* Tiles;
};

struct Map* CreateMap(
    const int width,
    const int height,
    const int tileWidth,
    const int tileHeight);
void DestroyMap(struct Map** map);
struct Tile* GetNeighbor(
    const struct Map* map,
    const struct Tile* tile,
    const enum Direction direction);
struct Tile* GetTile(const struct Map* map, const int x, const int y);
int GetTileCount(const struct Map* map);
size_t GetTileIndex(const struct Map* map, struct Tile* tile);
int InBounds(const struct Map* map, const int x, const int y);

#endif