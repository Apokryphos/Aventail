#include "actor.h"
#include "inventory.h"
#include "map_link.h"
#include "tile.h"

#include <stdio.h>
#include <string.h>

const int ERROR_MAP_FILE_LOAD = 1;

const int MAP_FILE_STRING_MAX_LENGTH = 64;
const int MAP_FILE_MAX_MAP_LINKS = 16;

//  ---------------------------------------------------------------------------
void dump_actor_info(const struct Actor* actor)
{
    printf(
        "[Actor] NAME: %s GID: %d POS: %d, %d COL: %d TYPE: %d CASH: %d ITEMS: %zu\n",
        actor->name,
        actor->tileset_id,
        actor->tile->x,
        actor->tile->y,
        actor->collision,
        actor->type,
        actor->cash,
        get_inventory_item_count(actor->inventory));
}

//  ---------------------------------------------------------------------------
void dump_map_link_info(const struct Tile* tile)
{
    struct MapLink* link = tile->link;
    if (link != NULL)
    {
        printf(
            "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
            link->dest_map,
            tile->x,
            tile->y,
            link->dest_x,
            link->dest_y);
    }
}

//  ---------------------------------------------------------------------------
int validate_map_file_map_link_count(const int count)
{
    if (count < 0)
    {
        fprintf(stderr, "Map link count is negative.\n");
        return 1;
    }

    if (count > MAP_FILE_MAX_MAP_LINKS)
    {
        fprintf(
            stderr,
            "Map link count of %d is greater than max of %d.\n",
            count,
            MAP_FILE_MAX_MAP_LINKS);
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int validate_map_file_string_len(const int len)
{
    if (len == 0 || len > MAP_FILE_STRING_MAX_LENGTH)
    {
        fprintf(stderr, "Map file string length is not a valid length.\n");
        fprintf(stderr, "Length: %d\n", len);
        fprintf(stderr, "Max Length: %d\n", MAP_FILE_STRING_MAX_LENGTH);
        return 1;
    }

    return 0;
}