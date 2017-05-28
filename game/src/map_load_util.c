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
void LoadActorsFromFile(FILE* file, struct Map* map, struct ActorList* actors)
{
    assert(map != NULL);
    assert(actors != NULL);

    int actorCount = 0;
    fread(&actorCount, sizeof(int), 1, file);
    printf("Loading %d actors...\n", actorCount);

    for (int a = 0; a < actorCount; ++a)
    {
        int tilesetId = 0;
        int tileX = 0;
        int tileY = 0;
        int collision = 0;
        int type = 0;
        int nameLen = 0;
        char* name = NULL;
        int cash = 0;

        fread(&tilesetId, sizeof(int), 1, file);
        fread(&tileX, sizeof(int), 1, file);
        fread(&tileY, sizeof(int), 1, file);
        fread(&collision, sizeof(int), 1, file);
        fread(&type, sizeof(int), 1, file);
        fread(&cash, sizeof(int), 1, file);

        fread(&nameLen, sizeof(int), 1, file);
        assert(nameLen <= MAX_ACTOR_NAME_STRING_LENGTH);

        name = malloc(sizeof(char) * nameLen + 1);
        fread(name, sizeof(char), nameLen, file);
        name[nameLen] = '\0';

        struct Actor* actor = create_actor(map, name, tileX, tileY, tilesetId);

        size_t itemCount = 0;
        fread(&itemCount, sizeof(int), 1, file);
        for (size_t n = 0; n < itemCount; ++n)
        {
            fread(&nameLen, sizeof(int), 1, file);

            char* itemName = malloc(sizeof(char) * nameLen + 1);
            fread(itemName, sizeof(char), nameLen, file);
            itemName[nameLen] = '\0';

            struct Item* item = create_item(itemName);

            LoadItemDefinition(item);

            add_item_to_inventory(actor->inventory, item);

            free(itemName);
        }

        actor->collision = collision;
        actor->type = (enum ActorType)type;

        if (actor->type == ACTOR_TYPE_DOOR)
        {
            actor->on_touch = &ActivateDoor;
        }

        if (actor->type == ACTOR_TYPE_CONTAINER)
        {
            actor->on_touch = &ActivateContainer;
        }

        actor->cash = cash;

        LoadActorDefinition(actor);

        printf(
            "[Actor] NAME: %s GID: %d POS: %d, %d COL: %d TYPE: %d CASH: %d ITEMS: %zu\n",
            actor->name,
            actor->tileset_id,
            actor->tile->X,
            actor->tile->Y,
            actor->collision,
            actor->type,
            actor->cash,
            get_inventory_item_count(actor->inventory));

        add_actor_to_actor_list_back(actors, actor);

        free(name);
    }
}

//  ---------------------------------------------------------------------------
struct Map* LoadMapFromFile(FILE* file)
{
    printf("Loading map header...\n");
    int width, height, tileWidth, tileHeight;
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);
    fread(&tileWidth, sizeof(int), 1, file);
    fread(&tileHeight, sizeof(int), 1, file);

    struct Map* map = create_map(width, height, tileWidth, tileHeight);

    int tileCount = get_map_tile_count(map);
    printf("Loading %d tiles...\n", tileCount);

    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &map->tiles[t];
        fread(&tile->TilesetId, sizeof(int), 1, file);
        fread(&tile->Collision, sizeof(int), 1, file);
    }

    int mapLinkCount = 0;
    fread(&mapLinkCount, sizeof(int), 1, file);
    printf("Loading %d map links...\n", mapLinkCount);
    for (int n = 0; n < mapLinkCount; ++n)
    {
        char* destMap = NULL;
        int destMapLen = 0;
        int tileX, tileY, destX, destY;

        fread(&tileX, sizeof(int), 1, file);
        fread(&tileY, sizeof(int), 1, file);

        fread(&destMapLen, sizeof(int), 1, file);
        assert(destMapLen <= MAX_DEST_MAP_STRING_LENGTH);

        destMap = malloc(sizeof(char) * destMapLen + 1);
        fread(destMap, sizeof(char), destMapLen, file);
        destMap[destMapLen] = '\0';

        fread(&destX, sizeof(int), 1, file);
        fread(&destY, sizeof(int), 1, file);

        struct MapLink* link = create_map_link(destMap, destX, destY);

        free(destMap);

        struct Tile* tile = &map->tiles[tileY * map->width + tileX];
        tile->Link = link;

        printf(
            "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
            link->dest_map,
            tileX,
            tileY,
            link->dest_x,
            link->dest_y);
    }

    return map;
}