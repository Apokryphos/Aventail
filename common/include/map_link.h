#ifndef MAP_LINK_HEADER_INCLUDED
#define MAP_LINK_HEADER_INCLUDED

struct MapLink
{
    char* DestMap;
    int DestX;
    int DestY;
};

/*
    Allocates a MapLink. Free with DestroyMapLink.
    A MapLink assigned to the Link member of a Tile struct will be freed
    when that tile's map is destroyed.
*/
struct MapLink* CreateMapLink(
    const char* destMap,
    const int destX,
    const int destY);
/**
    Frees the specified MapLink. Afterwards the specified pointer will be
    set to NULL.
*/
void DestroyMapLink(struct MapLink** link);

#endif