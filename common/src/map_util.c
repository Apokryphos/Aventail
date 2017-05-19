#include "actor.h"
#include "actor_funcs.h"
#include "actor_list.h"
#include "map.h"
#include "map_link.h"
#include "tile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int MAX_DEST_MAP_STRING_LENGTH = 256;

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

        fread(&tilesetId, sizeof(int), 1, file);
        fread(&tileX, sizeof(int), 1, file);
        fread(&tileY, sizeof(int), 1, file);
        fread(&collision, sizeof(int), 1, file);
        fread(&type, sizeof(int), 1, file);

        struct Actor* actor = CreateActor(map, tileX, tileY, tilesetId);
        actor->Collision = collision;
        actor->Type = (enum ActorType)type;

        if (actor->Type == ACTOR_TYPE_DOOR)
        {
            actor->OnTouch = &ActivateDoor;
        }

        printf(
            "[Actor] GID: %d POS: %d, %d COL: %d TYPE: %d\n",
            actor->TilesetId,
            actor->Tile->X,
            actor->Tile->Y,
            actor->Collision,
            actor->Type);

        AddActor(actors, actor);
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

//  ---------------------------------------------------------------------------
void SaveActorsToFile(FILE* file, const struct ActorList* actors)
{
    assert(actors != NULL);

    if (actors->Count > 0)
    {
        printf("Saving %d actors...\n", actors->Count);
        
        fwrite(&actors->Count, sizeof(int), 1, file);

        struct ActorListNode* node = actors->First;
        while (node != NULL)
        {
            const struct Actor* actor = node->Actor;

            int type = (int)actor->Type;

            fwrite(&actor->TilesetId, sizeof(int), 1, file);
            fwrite(&actor->Tile->X, sizeof(int), 1, file);
            fwrite(&actor->Tile->Y, sizeof(int), 1, file);
            fwrite(&actor->Collision, sizeof(int), 1, file);
            fwrite(&type, sizeof(int), 1, file);

            node = node->Next;

            printf(
                "[Actor] GID: %d POS: %d, %d COL: %d TYPE: %d\n",
                actor->TilesetId,
                actor->Tile->X,
                actor->Tile->Y,
                actor->Collision,
                actor->Type);
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