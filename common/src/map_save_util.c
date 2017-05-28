#include "actor.h"
#include "actor_list.h"
#include "inventory.h"
#include "item.h"
#include "map.h"
#include "map_file.h"
#include "map_link.h"
#include "tile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
void save_actors_to_file(FILE* file, const struct ActorList* actors)
{
    assert(actors != NULL);

    if (actors->count > 0)
    {
        printf("Saving %d actors...\n", actors->count);

        fwrite(&actors->count, sizeof(int), 1, file);

        struct ActorListNode* node = actors->front;
        while (node != NULL)
        {
            const struct Actor* actor = node->actor;

            int type = (int)actor->type;

            fwrite(&actor->tileset_id, sizeof(int), 1, file);
            fwrite(&actor->tile->x, sizeof(int), 1, file);
            fwrite(&actor->tile->y, sizeof(int), 1, file);
            fwrite(&actor->collision, sizeof(int), 1, file);
            fwrite(&type, sizeof(int), 1, file);
            fwrite(&actor->cash, sizeof(int), 1, file);

            size_t name_len = strlen(actor->name);
            assert(name_len <= MAX_ACTOR_NAME_STRING_LENGTH);

            fwrite(&name_len, sizeof(int), 1, file);
            fwrite(actor->name, sizeof(char), name_len, file);

            size_t item_write_count = 0;
            size_t item_count = get_inventory_item_count(actor->inventory);
            fwrite(&item_count, sizeof(int), 1, file);
            for (size_t n = 0; n < MAX_INVENTORY_ITEMS; ++n)
            {
                struct Item* item = actor->inventory->items[n];

                if (item != NULL)
                {
                    assert(item_write_count < item_count);

                    name_len = strlen(item->name);
                    assert(name_len <= MAX_ACTOR_NAME_STRING_LENGTH);

                    fwrite(&name_len, sizeof(int), 1, file);
                    fwrite(item->name, sizeof(char), name_len, file);

                    item_write_count++;
                }
            }

            node = node->next;

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
    }
}

//  ---------------------------------------------------------------------------
void save_map_to_file(FILE* file, const struct Map* map)
{
    printf("Saving map header...\n");
    fwrite(&map->width, sizeof(int), 1, file);
    fwrite(&map->height, sizeof(int), 1, file);
    fwrite(&map->tile_height, sizeof(int), 1, file);
    fwrite(&map->tile_height, sizeof(int), 1, file);

    int map_link_count = 0;
    int tile_count = get_map_tile_count(map);
    printf("Saving %d tiles...\n", tile_count);
    for (int t = 0; t < tile_count; ++t)
    {
        struct Tile* tile = &map->tiles[t];
        fwrite(&tile->tileset_id, sizeof(int), 1, file);
        fwrite(&tile->collision, sizeof(int), 1, file);

        if (tile->link != NULL)
        {
            ++map_link_count;
        }
    }

    printf("Saving %d map links...\n", map_link_count);
    fwrite(&map_link_count, sizeof(int), 1, file);
    for (int t = 0; t < tile_count; ++t)
    {
        struct Tile* tile = &map->tiles[t];
        struct MapLink* link = tile->link;

        if (link != NULL)
        {
            size_t dest_map_len = strlen(link->dest_map);
            assert(dest_map_len <= MAX_DEST_MAP_STRING_LENGTH);

            fwrite(&tile->x, sizeof(int), 1, file);
            fwrite(&tile->y, sizeof(int), 1, file);
            fwrite(&dest_map_len, sizeof(int), 1, file);
            fwrite(link->dest_map, sizeof(char), dest_map_len, file);
            fwrite(&link->dest_x, sizeof(int), 1, file);
            fwrite(&link->dest_y, sizeof(int), 1, file);

            printf(
                "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
                link->dest_map,
                tile->x,
                tile->y,
                link->dest_x,
                link->dest_y);
        }
    }
}