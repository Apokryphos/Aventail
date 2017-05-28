#include "map_link.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct MapLink* create_map_link(
    const char* dest_map,
    const int dest_x,
    const int dest_y)
{
    assert(dest_map != NULL);
    assert(dest_x >= 0);
    assert(dest_y >= 0);

    struct MapLink* link = malloc(sizeof(struct MapLink));
    link->dest_map = strdup(dest_map);
    link->dest_x = dest_x;
    link->dest_y = dest_y;

    return link;
}

//  ---------------------------------------------------------------------------
void destroy_map_link(struct MapLink** link)
{
    if (*link == NULL)
    {
        return;
    }

     if ((*link)->dest_map != NULL)
    {
        free((*link)->dest_map);
    }

    free(*link);
    *link = NULL;
}