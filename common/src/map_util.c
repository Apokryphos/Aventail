#include "actor.h"
#include "actor_list.h"
#include "map.h"
#include "tile.h"
#include <assert.h>
#include <stdio.h>

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

        fread(&tilesetId, sizeof(int), 1, file);
        fread(&tileX, sizeof(int), 1, file);
        fread(&tileY, sizeof(int), 1, file);

        struct Actor* actor = CreateActor(map, tileX, tileY, tilesetId);

        printf(
            "[Actor] GID: %d POS: %d, %d\n",
            actor->TilesetId,
            actor->Tile->X,
            actor->Tile->Y);

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

            fwrite(&actor->TilesetId, sizeof(int), 1, file);
            fwrite(&actor->Tile->X, sizeof(int), 1, file);
            fwrite(&actor->Tile->Y, sizeof(int), 1, file);

            node = node->Next;

            printf(
                "[Actor] GID: %d POS: %d, %d\n",
                actor->TilesetId,
                actor->Tile->X,
                actor->Tile->Y);
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

    int tileCount = GetTileCount(map);
    printf("Saving %d tiles...\n", tileCount);
    for (int t = 0; t < tileCount; ++t)
    {
        struct Tile* tile = &map->Tiles[t];
        fwrite(&tile->TilesetId, sizeof(int), 1, file);
        fwrite(&tile->Collision, sizeof(int), 1, file);
    }
}