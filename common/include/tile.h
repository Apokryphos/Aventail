#ifndef TILE_HEADER_INCLUDED
#define TILE_HEADER_INCLUDED

struct MapLink;

struct Tile
{
    int collision;
    int flip_flags;
    int light;
    int tileset_id;
    int x;
    int y;
    struct MapLink* link;
};

#endif