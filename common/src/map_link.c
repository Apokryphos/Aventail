#include "map_link.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct MapLink* CreateMapLink(char* destMap, int destX, int destY)
{
    assert(destMap != NULL);
    assert(destX >= 0);
    assert(destY >= 0);

    struct MapLink* link = malloc(sizeof(struct MapLink));
    link->DestMap = strdup(destMap);
    link->DestX = destX;
    link->DestY = destY;
    
    return link;
}

//  ---------------------------------------------------------------------------
void DestroyMapLink(struct MapLink** link)
{
    if (*link == NULL)
    {
        return;
    }

     if ((*link)->DestMap != NULL)
    {
        free((*link)->DestMap);
    }   

    free(*link);
    *link = NULL;
}