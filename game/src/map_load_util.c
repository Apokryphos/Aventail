#include "actor.h"
#include "actor_ai.h"
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
int load_actors_from_file(FILE* file, struct Map* map, struct ActorList* actors)
{
    assert(map != NULL);
    assert(actors != NULL);

    int actor_count = 0;
    fread(&actor_count, sizeof(int), 1, file);

    for (int a = 0; a < actor_count; ++a)
    {
        int tileset_id = 0;
        int flip_flags = 0;
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
        fread(&flip_flags, sizeof(int), 1, file);
        fread(&type, sizeof(int), 1, file);
        fread(&cash, sizeof(int), 1, file);

        fread(&name_len, sizeof(int), 1, file);
        if (validate_map_file_string_len(name_len))
        {
            fprintf(stderr, "Actor name length is not valid.\n");
            return 1;
        }
        name = malloc(sizeof(char) * name_len + 1);
        fread(name, sizeof(char), name_len, file);
        name[name_len] = '\0';

        struct Actor* actor = create_actor(map, name, tile_x, tile_y, tileset_id, flip_flags);

        size_t itemCount = 0;
        fread(&itemCount, sizeof(int), 1, file);
        for (size_t n = 0; n < itemCount; ++n)
        {
            fread(&name_len, sizeof(int), 1, file);
            if (validate_map_file_string_len(name_len))
            {
                fprintf(stderr, "Item name length is not valid.\n");
                return 1;
            }
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

        if (actor->type == ACTOR_TYPE_VILLAIN)
        {
            actor->ai = create_actor_ai();
        }

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

        //dump_actor_info(actor);

        add_actor_to_actor_list_back(actors, actor);

        free(name);
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int load_map_links_from_file(FILE* file, struct Map* map)
{
    int map_link_count = 0;
    fread(&map_link_count, sizeof(int), 1, file);

    if (validate_map_file_map_link_count(map_link_count) != 0)
    {
        return ERROR_MAP_FILE_LOAD;
    }

    for (int n = 0; n < map_link_count; ++n)
    {
        char* dest_map = NULL;
        int dest_map_len = 0;
        int tile_x, tile_y, dest_x, dest_y;

        fread(&tile_x, sizeof(int), 1, file);
        fread(&tile_y, sizeof(int), 1, file);

        fread(&dest_map_len, sizeof(int), 1, file);
        if (validate_map_file_string_len(dest_map_len))
        {
            fprintf(stderr, "Destination map name length is not valid.\n");
            return ERROR_MAP_FILE_LOAD;
        }

        dest_map = malloc(sizeof(char) * dest_map_len + 1);
        fread(dest_map, sizeof(char), dest_map_len, file);
        dest_map[dest_map_len] = '\0';

        fread(&dest_x, sizeof(int), 1, file);
        fread(&dest_y, sizeof(int), 1, file);

        struct MapLink* link = create_map_link(dest_map, dest_x, dest_y);

        free(dest_map);

        struct Tile* tile = &map->tiles[tile_y * map->width + tile_x];
        tile->link = link;

        //dump_map_link_info(tile);
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int load_map_from_file(
    FILE* file,
    struct Map** map,
    struct ActorList** actors)
{
    int width, height, tile_width, tile_height, sunlight;
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    fread(&tile_width, sizeof(int), 1, file);
    fread(&tile_height, sizeof(int), 1, file);
    fread(&sunlight, sizeof(int), 1, file);

    *map = create_map(width, height, tile_width, tile_height);
    if (*map == NULL)
    {
        fprintf(stderr, "Failed to create map.\n");
        return ERROR_MAP_FILE_LOAD;
    }

    (*map)->sunlight = sunlight;

    int tile_count = get_map_tile_count(*map);
    for (int t = 0; t < tile_count; ++t)
    {
        struct Tile* tile = &(*map)->tiles[t];
        fread(&tile->tileset_id, sizeof(int), 1, file);
        fread(&tile->collision, sizeof(int), 1, file);
        fread(&tile->flip_flags, sizeof(int), 1, file);
    }

    if (load_map_links_from_file(file, *map) != 0)
    {
        fprintf(stderr, "Failed to load map links.\n");
        destroy_map(map);
        return ERROR_MAP_FILE_LOAD;
    }

    *actors = create_actor_list();
    if (*actors == NULL)
    {
        fprintf(stderr, "Failed to create actor list.\n");
        destroy_map(map);
        return ERROR_MAP_FILE_LOAD;
    }

    if (load_actors_from_file(file, *map, *actors) != 0)
    {
        fprintf(stderr, "Failed to load map actors.\n");
        destroy_map(map);
        destroy_actor_list(actors);
        return ERROR_MAP_FILE_LOAD;
    }

    return 0;
}