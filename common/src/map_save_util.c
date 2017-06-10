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
            fwrite(&actor->flip_flags, sizeof(int), 1, file);
            fwrite(&type, sizeof(int), 1, file);
            fwrite(&actor->cash, sizeof(int), 1, file);

            size_t name_len = strlen(actor->name);
            if (validate_map_file_string_len(name_len))
            {
                fprintf(stderr, "Actor map name length is not valid.\n");
                exit(1);
            }
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
                    if (validate_map_file_string_len(name_len))
                    {
                        fprintf(stderr, "Item map name length is not valid.\n");
                        exit(1);
                    }
                    fwrite(&name_len, sizeof(int), 1, file);
                    fwrite(item->name, sizeof(char), name_len, file);

                    item_write_count++;
                }
            }

            node = node->next;

            dump_actor_info(actor);
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
    fwrite(&map->sunlight, sizeof(int), 1, file);

    int map_link_count = 0;
    int tile_count = get_map_tile_count(map);
    printf("Saving %d tiles...\n", tile_count);
    for (int t = 0; t < tile_count; ++t)
    {
        struct Tile* tile = &map->tiles[t];
        fwrite(&tile->tileset_id, sizeof(int), 1, file);
        fwrite(&tile->collision, sizeof(int), 1, file);
        fwrite(&tile->flip_flags, sizeof(int), 1, file);

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
            int dest_map_len = strlen(link->dest_map);
            if (validate_map_file_string_len(dest_map_len))
            {
                fprintf(stderr, "Destination map name length is not valid.\n");
                exit(1);
            }

            fwrite(&tile->x, sizeof(int), 1, file);
            fwrite(&tile->y, sizeof(int), 1, file);
            fwrite(&dest_map_len, sizeof(int), 1, file);
            fwrite(link->dest_map, sizeof(char), dest_map_len, file);
            fwrite(&link->dest_x, sizeof(int), 1, file);
            fwrite(&link->dest_y, sizeof(int), 1, file);

            dump_map_link_info(tile);
        }
    }
}