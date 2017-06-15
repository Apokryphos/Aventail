#include "actor_list.h"
#include "map.h"
#include "zone.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void add_actor_to_zone(struct Zone* zone, struct Actor* actor)
{
    add_actor_to_actor_list_back(zone->actors, actor);
}

//  ---------------------------------------------------------------------------
struct Zone* create_zone()
{
    struct Zone* zone = malloc(sizeof(struct Zone));
    zone->actors = create_actor_list();
    zone->map = NULL;
    return zone;
}

//  ---------------------------------------------------------------------------
void destroy_zone(struct Zone** zone)
{
    if (*zone == NULL)
    {
        return;
    }

    destroy_actor_list(&(*zone)->actors);

    if ((*zone)->map != NULL)
    {
        destroy_map(&(*zone)->map);
    }

    free(*zone);
    *zone = NULL;
}

//  ---------------------------------------------------------------------------
void remove_actor_from_zone(struct Zone* zone, struct Actor* actor)
{
    remove_actor_from_actor_list(zone->actors, actor);
}