#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "map.h"
#include "tile.h"
#include "tileset.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <string.h>

static const int RenderScale = 2;

static struct Tileset FontTileset;
static struct Tileset MapTileset;

//  ---------------------------------------------------------------------------
void DrawMap(
    SDL_Renderer* renderer, 
    struct Map* map, 
    struct ActorList* actors)
{
    assert(renderer != NULL);
    assert(map != NULL);
    assert(MapTileset.Texture != NULL);

    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = map->TileWidth * RenderScale;
    destRect.h = map->TileHeight * RenderScale;

    for (int y = 0; y < map->Height; ++y)
    {
        for (int x = 0; x < map->Width; ++x)
        {
            int tilesetId = map->Tiles[y * map->Width + x].TilesetId;

            if (tilesetId == 0)
            {
                continue;
            }

            GetTilesetRect(&MapTileset, tilesetId, &sourceRect);
            
            destRect.x = x * destRect.w;
            destRect.y = y * destRect.h;
            SDL_RenderCopy(renderer, MapTileset.Texture, &sourceRect, &destRect);
        }
    }

    struct ActorListNode* actorNode = actors->Last;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        if (actor->Tile != NULL)
        {
            GetTilesetRect(&MapTileset, actor->TilesetId, &sourceRect);

            destRect.x = actor->Tile->X * destRect.w;
            destRect.y = actor->Tile->Y * destRect.h;
            SDL_RenderCopy(renderer, MapTileset.Texture, &sourceRect, &destRect);  
        }

        actorNode = actorNode->Previous;
    }
}

//  ---------------------------------------------------------------------------
void DrawText(
    SDL_Renderer* renderer, 
    const char* text,
    int x,
    int y)
{
    assert(renderer != NULL);
    assert(text != NULL);
    assert(FontTileset.Texture != NULL);

    SDL_Rect destRect = 
    { 
        .x = x, 
        .y = y, 
        .w = FontTileset.TileWidth * RenderScale, 
        .h = FontTileset.TileHeight *  RenderScale
    };
    SDL_Rect sourceRect;

    int textLength = strlen(text);
    for (int t = 0; t < textLength; ++t)
    {
        int c = (int)text[t];
        if (c > 32 && c < 127)
        {
            GetTilesetRect(&FontTileset, c - 32, &sourceRect);
            SDL_RenderCopy(renderer, FontTileset.Texture, &sourceRect, &destRect);
        }

        destRect.x += destRect.w;
    }
}

//  ---------------------------------------------------------------------------
void DrawTextAlpha(
    SDL_Renderer* renderer, 
    const char* text,
    int x,
    int y,
    int alpha)
{
    SDL_SetTextureAlphaMod(FontTileset.Texture, alpha);
    DrawText(renderer, text, x, y);
    SDL_SetTextureAlphaMod(FontTileset.Texture, 255);
}

//  ---------------------------------------------------------------------------
void GetTileRect(struct Map* map, struct Tile* tile, SDL_Rect* rect)
{
    assert(map != NULL);
    assert(tile != NULL);
    assert(rect != NULL);

    (*rect).x = tile->X * map->TileWidth * RenderScale;
    (*rect).y = tile->Y * map->TileHeight * RenderScale;
    (*rect).w = map->TileWidth * RenderScale;
    (*rect).h = map->TileHeight * RenderScale;
}

//  ---------------------------------------------------------------------------
int GfxInit(struct Game* game)
{
    assert(FontTileset.Texture == NULL);
    assert(MapTileset.Texture == NULL);

    LoadTileset(game, &FontTileset, "font");
    if (FontTileset.Texture == NULL)
    {
        printf("Failed to load font tileset texture.\n");
        return -1;
    }

    FontTileset.TileWidth = 5;
    FontTileset.TileHeight = 12;

    LoadTileset(game, &MapTileset, "tileset");
    if (MapTileset.Texture == NULL)
    {
        printf("Failed to load map tileset texture.\n");
        return -1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void GfxShutdown()
{
    SDL_DestroyTexture(FontTileset.Texture);
    SDL_DestroyTexture(MapTileset.Texture);
}

//  ---------------------------------------------------------------------------
void MeasureText(const char* text, int* width, int* height)
{
    assert(text != NULL);

    *width = strlen(text) * FontTileset.TileWidth * RenderScale;
    *height = FontTileset.TileHeight * RenderScale;
}