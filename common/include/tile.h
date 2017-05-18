#ifndef TILE_HEADER_INCLUDED
#define TILE_HEADER_INCLUDED

struct MapLink;

struct Tile
{
    int Collision;
    int TilesetId;
    int X;
    int Y;
    struct MapLink* Link;
};

#endif