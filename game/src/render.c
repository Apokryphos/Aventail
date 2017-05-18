#include "actor.h"
#include "actor_list.h"
#include "map.h"
#include "tile.h"
#include "tileset.h"
#include <SDL2/SDL.h>
#include <assert.h>

//  ---------------------------------------------------------------------------
void DrawMap(
    SDL_Renderer* renderer, 
    struct Map* map, 
    struct ActorList* actors,
    struct Tileset* tileset)
{
    assert(renderer != NULL);
    assert(map != NULL);
    assert(tileset != NULL);

    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = 32;
    destRect.h = 32;

    for (int y = 0; y < map->Height; ++y)
    {
        for (int x = 0; x < map->Width; ++x)
        {
            int tilesetId = map->Tiles[y * map->Width + x].TilesetId;

            if (tilesetId == 0)
            {
                continue;
            }

            GetTilesetRect(tileset, tilesetId, &sourceRect);
            
            destRect.x = x * destRect.w;
            destRect.y = y * destRect.h;
            SDL_RenderCopy(renderer, tileset->Texture, &sourceRect, &destRect);
        }
    }

    struct ActorListNode* actorNode = actors->First;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        if (actor->Tile != NULL)
        {
            GetTilesetRect(tileset, actor->TilesetId, &sourceRect);

            destRect.x = actor->Tile->X * destRect.w;
            destRect.y = actor->Tile->Y * destRect.h;
            SDL_RenderCopy(renderer, tileset->Texture, &sourceRect, &destRect);  
        }

        actorNode = actorNode->Next;
    }
}