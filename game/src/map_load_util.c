#include "actor.h"
#include "actor_defs.h"
#include "actor_funcs.h"
#include "actor_list.h"
#include "inventory.h"
#include "item.h"
#include "item_defs.h"
#include "map.h"
#include "map_file.h"
#include "map_link.h"
#include "tile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
void load_actors_from_file(FILE* file, struct Map* map, struct ActorList* actors)
{
    assert(map != NULL);
    assert(actors != NULL);

    int actor_count = 0;
    fread(&actor_count, sizeof(int), 1, file);
    printf("Loading %d actors...\n", actor_count);

    for (int a = 0; a < actor_count; ++a)
    {
        int tileset_id = 0;
        int tile_x = 0;
        int tile_y = 0;
        int collision = 0;
        int type = 0;
        int name_len = 0;
        char* name = NULL;
        int cash = 0;

        fread(&tileset_id, sizeof(int), 1, file);
        fread(&tile_x, sizeof(int), 1, file);
        fread(&tile_y, sizeof(int), 1, file);
        fread(&collision, sizeof(int), 1, file);
        fread(&type, sizeof(int), 1, file);
        fread(&cash, sizeof(int), 1, file);

        fread(&name_len, sizeof(int), 1, file);
        assert(name_len <= MAX_ACTOR_NAME_STRING_LENGTH);

        name = malloc(sizeof(char) * name_len + 1);
        fread(name, sizeof(char), name_len, file);
        name[name_len] = '\0';

        struct Actor* actor = create_actor(map, name, tile_x, tile_y, tileset_id);

        size_t itemCount = 0;
        fread(&itemCount, sizeof(int), 1, file);
        for (size_t n = 0; n < itemCount; ++n)
        {
            fread(&name_len, sizeof(int), 1, file);

            char* item_name = malloc(sizeof(char) * name_len + 1);
            fread(item_name, sizeof(char), name_len, file);
            item_name[name_len] = '\0';

            struct Item* item = create_item(item_name);

            load_item_definition(item);

            add_item_to_inventory(actor->inventory, item);

            free(item_name);
        }

        actor->collision = collision;
        actor->type = (enum ActorType)type;

        if (actor->type == ACTOR_TYPE_DOOR)
        {
            actor->on_touch = &activate_door;
        }

        if (actor->type == ACTOR_TYPE_CONTAINER)
        {
            actor->on_touch = &activate_container;
        }

        actor->cash = cash;

        load_actor_definition(actor);

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

        add_actor_to_actor_list_back(actors, actor);

        free(name);
    }
}

//  ---------------------------------------------------------------------------
struct Map* load_map_from_file(FILE* file)
{
    printf("Loading map header...\n");
    int width, height, tile_width, tile_height;
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    fread(&tile_width, sizeof(int), 1, file);
    fread(&tile_height, sizeof(int), 1, file);

    struct Map* map = create_map(width, height, tile_width, tile_height);

    int tile_count = get_map_tile_count(map);
    printf("Loading %d tiles...\n", tile_count);

    for (int t = 0; t < tile_count; ++t)
    {
        struct Tile* tile = &map->tiles[t];
        fread(&tile->tileset_id, sizeof(int), 1, file);
        fread(&tile->collision, sizeof(int), 1, file);
    }

    int map_link_count = 0;
    fread(&map_link_count, sizeof(int), 1, file);
    printf("Loading %d map links...\n", map_link_count);
    for (int n = 0; n < map_link_count; ++n)
    {
        char* dest_map = NULL;
        int dest_map_len = 0;
        int tile_x, tile_y, dest_x, dest_y;

        fread(&tile_x, sizeof(int), 1, file);
        fread(&tile_y, sizeof(int), 1, file);

        fread(&dest_map_len, sizeof(int), 1, file);
        assert(dest_map_len <= MAX_DEST_MAP_STRING_LENGTH);

        dest_map = malloc(sizeof(char) * dest_map_len + 1);
        fread(dest_map, sizeof(char), dest_map_len, file);
        dest_map[dest_map_len] = '\0';

        fread(&dest_x, sizeof(int), 1, file);
        fread(&dest_y, sizeof(int), 1, file);

        struct MapLink* link = create_map_link(dest_map, dest_x, dest_y);

        free(dest_map);

        struct Tile* tile = &map->tiles[tile_y * map->width + tile_x];
        tile->link = link;

        printf(
            "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
            link->dest_map,
            tile_x,
            tile_y,
            link->dest_x,
            link->dest_y);
    }

    return map;
}