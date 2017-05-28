#ifndef MAP_LINK_HEADER_INCLUDED
#define MAP_LINK_HEADER_INCLUDED

struct MapLink
{
    char* dest_map;
    int dest_x;
    int dest_y;
};

/*
    Allocates a MapLink. Free with destroy_map_link.
    A MapLink assigned to the Link member of a Tile struct will be freed
    when that tile's map is destroyed.
*/
struct MapLink* create_map_link(
    const char* dest_map,
    const int dest_x,
    const int dest_y);
/**
    Frees the specified MapLink. Afterwards the specified pointer will be
    set to NULL.
*/
void destroy_map_link(struct MapLink** link);

#endif