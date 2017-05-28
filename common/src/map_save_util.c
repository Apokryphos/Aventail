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
void SaveActorsToFile(FILE* file, const struct ActorList* actors)
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
            fwrite(&actor->tile->X, sizeof(int), 1, file);
            fwrite(&actor->tile->Y, sizeof(int), 1, file);
            fwrite(&actor->collision, sizeof(int), 1, file);
            fwrite(&type, sizeof(int), 1, file);
            fwrite(&actor->cash, sizeof(int), 1, file);

            size_t nameLen = strlen(actor->name);
            assert(nameLen <= MAX_ACTOR_NAME_STRING_LENGTH);

            fwrite(&nameLen, sizeof(int), 1, file);
            fwrite(actor->name, sizeof(char), nameLen, file);

            size_t itemsWritten = 0;
            size_t itemCount = GetInventoryItemCount(actor->inventory);
            fwrite(&itemCount, sizeof(int), 1, file);
            for (size_t n = 0; n < MaxInventoryItems; ++n)
            {
                struct Item* item = actor->inventory->Items[n];

                if (item != NULL)
                {
                    assert(itemsWritten < itemCount);

                    nameLen = strlen(item->Name);
                    assert(nameLen <= MAX_ACTOR_NAME_STRING_LENGTH);

                    fwrite(&nameLen, sizeof(int), 1, file);
                    fwrite(item->Name, sizeof(char), nameLen, file);

                    itemsWritten++;
                }
            }

            node = node->next;

            printf(
                "[Actor] NAME: %s GID: %d POS: %d, %d COL: %d TYPE: %d CASH: %d ITEMS: %zu\n",
                actor->name,
                actor->tileset_id,
                actor->tile->X,
                actor->tile->Y,
                actor->collision,
                actor->type,
                actor->cash,
                GetInventoryItemCount(actor->inventory));
        }
    }
}

//  ---------------------------------------------------------------------------
void SaveMapToFile(FILE* file, const struct Map* map)
{
    printf("Saving map header...\n");
    fwrite(&map->Width, sizeof(int), 1, file);
    fwrite(&map->Height, sizeof(int), 1, file);
    fwrite(&map->TileHeight, sizeof(int), 1, file);
    fwrite(&map->TileHeight, sizeof(int), 1, file);

    int mapLinkCount = 0;
    int tileCount = GetTileCount(map);
    printf("Saving %d tiles...\n", tileCount);
    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &map->Tiles[t];
        fwrite(&tile->TilesetId, sizeof(int), 1, file);
        fwrite(&tile->Collision, sizeof(int), 1, file);

        if (tile->Link != NULL)
        {
            ++mapLinkCount;
        }
    }

    printf("Saving %d map links...\n", mapLinkCount);
    fwrite(&mapLinkCount, sizeof(int), 1, file);
    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &map->Tiles[t];
        struct MapLink* link = tile->Link;

        if (link != NULL)
        {
            size_t destMapLen = strlen(link->DestMap);
            assert(destMapLen <= MAX_DEST_MAP_STRING_LENGTH);

            fwrite(&tile->X, sizeof(int), 1, file);
            fwrite(&tile->Y, sizeof(int), 1, file);
            fwrite(&destMapLen, sizeof(int), 1, file);
            fwrite(link->DestMap, sizeof(char), destMapLen, file);
            fwrite(&link->DestX, sizeof(int), 1, file);
            fwrite(&link->DestY, sizeof(int), 1, file);

            printf(
                "[MapLink] MAP: %s POS: %d, %d DEST: %d, %d\n",
                link->DestMap,
                tile->X,
                tile->Y,
                link->DestX,
                link->DestY);
        }
    }
}