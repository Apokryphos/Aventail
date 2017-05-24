#include "actor.h"
#include "actor_funcs.h"
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

        struct Actor* actor = CreateActor(map, name, tileX, tileY, tilesetId);

        size_t itemCount = 0;
        fread(&itemCount, sizeof(int), 1, file);
        for (size_t n = 0; n < itemCount; ++n)
        {
            fread(&nameLen, sizeof(int), 1, file);

            char* itemName = malloc(sizeof(char) * nameLen + 1);
            fread(itemName, sizeof(char), nameLen, file);
            itemName[nameLen] = '\0';

            struct Item* item = CreateItem(itemName);

            AddInventoryItem(actor->Inventory, item);

            free(itemName);
        }

        actor->Collision = collision;
        actor->Type = (enum ActorType)type;

        if (actor->Type == ACTOR_TYPE_DOOR)
        {
            actor->OnTouch = &ActivateDoor;
        }

        if (actor->Type == ACTOR_TYPE_CONTAINER)
        {
            actor->OnTouch = &ActivateContainer;
        }

        actor->Cash = cash;

        printf(
            "[Actor] NAME: %s GID: %d POS: %d, %d COL: %d TYPE: %d CASH: %d ITEMS: %zu\n",
            actor->Name,
            actor->TilesetId,
            actor->Tile->X,
            actor->Tile->Y,
            actor->Collision,
            actor->Type,
            actor->Cash,
            GetInventoryItemCount(actor->Inventory));

        AddActorToBack(actors, actor);

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

    struct Map* map = CreateMap(width, height, tileWidth, tileHeight);

    int tileCount = GetTileCount(map);
    printf("Loading %d tiles...\n", tileCount);
    
    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &map->Tiles[t];
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

        struct MapLink* link = CreateMapLink(destMap, destX, destY);

        free(destMap);

        struct Tile* tile = &map->Tiles[tileY * map->Width + tileX];
        tile->Link = link;

        printf(
            "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
            link->DestMap,
            tileX,
            tileY,
            link->DestX,
            link->DestY);
    }

    return map;
}