#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "map.h"
#include "panel.h"
#include "tile.h"
#include "tileset.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <string.h>

//  SDL_RenderSetScale scale amount
static const int RenderScale = 2;

static struct Tileset FontTileset;
static struct Tileset GuiTileset;
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
    destRect.w = map->TileWidth;
    destRect.h = map->TileHeight;

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
void DrawTilesetTile(SDL_Renderer* renderer, int tilesetId, int x, int y)
{
    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = MapTileset.TileWidth;
    destRect.h = MapTileset.TileHeight;

    GetTilesetRect(&MapTileset, tilesetId, &sourceRect);

    destRect.x = x;
    destRect.y = y;
    SDL_RenderCopy(renderer, MapTileset.Texture, &sourceRect, &destRect);
}

//  ---------------------------------------------------------------------------
void DrawPanel(SDL_Renderer* renderer, struct Panel* panel)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(MapTileset.Texture != NULL);

    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.x = panel->X;
    destRect.y = panel->Y;
    destRect.w = GuiTileset.TileWidth;
    destRect.h = GuiTileset.TileHeight;

    const int cornerTilesetId = 1065;
    const int sideTilesetId = 1072;

    SDL_Rect panelRect = { panel->X, panel->Y, panel->Width, panel->Height };
    SDL_SetRenderDrawColor(renderer, 42, 5, 3, 255);
    SDL_RenderFillRect(renderer, &panelRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    GetTilesetRect(&GuiTileset, sideTilesetId, &sourceRect);
    int startX = panel->X + destRect.w;
    int endX = panel->X + panel->Width - destRect.w;
    for (int x = startX; x < endX; x += destRect.w)
    {
        destRect.x = x;

        destRect.y = panel->Y;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            90,
            NULL,
            SDL_FLIP_NONE);

        destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            270,
            NULL,
            SDL_FLIP_NONE);
    }

    int startY = panel->Y + destRect.h;
    int endY = panel->Y + panel->Height - destRect.h;
    for (int y = startY; y < endY; y += destRect.h)
    {
        destRect.y = y;

        destRect.x = panel->X;
        SDL_RenderCopy(renderer, GuiTileset.Texture, &sourceRect, &destRect);

        destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
    }

    GetTilesetRect(&GuiTileset, cornerTilesetId, &sourceRect);

    //  Upper-left corner
    destRect.x = panel->X;
    destRect.y = panel->Y;
    SDL_RenderCopy(renderer, GuiTileset.Texture, &sourceRect, &destRect);

    //  Lower-left corner
    destRect.x = panel->X;
    destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_VERTICAL);

    //  Lower-right corner
    destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
    destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

    //  Upper-right corner
    destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
    destRect.y = panel->Y;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
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
        .w = FontTileset.TileWidth, 
        .h = FontTileset.TileHeight
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
void GetTileRect(struct Map* map, struct Tile* tile, SDL_Rect* rect, int scaled)
{
    assert(map != NULL);
    assert(tile != NULL);
    assert(rect != NULL);

    int scale = scaled ? RenderScale : 1;

    (*rect).x = tile->X * map->TileWidth * scale;
    (*rect).y = tile->Y * map->TileHeight * scale;
    (*rect).w = map->TileWidth * scale;
    (*rect).h = map->TileHeight * scale;
}

//  ---------------------------------------------------------------------------
int GfxInit(struct Game* game)
{
    assert(FontTileset.Texture == NULL);
    assert(GuiTileset.Texture == NULL);
    assert(MapTileset.Texture == NULL);

    SDL_RenderSetScale(game->Renderer, RenderScale, RenderScale);

    LoadTileset(game, &FontTileset, "font");
    if (FontTileset.Texture == NULL)
    {
        printf("Failed to load font tileset texture.\n");
        return -1;
    }

    FontTileset.TileWidth = 5;
    FontTileset.TileHeight = 12;

    LoadTileset(game, &GuiTileset, "tileset");
    if (GuiTileset.Texture == NULL)
    {
        printf("Failed to load GUI tileset texture.\n");
        return -1;
    }

    GuiTileset.TileWidth = 8;
    GuiTileset.TileHeight = 8;

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
    SDL_DestroyTexture(GuiTileset.Texture);
    SDL_DestroyTexture(MapTileset.Texture);
}

//  ---------------------------------------------------------------------------
void MeasureText(const char* text, int* width, int* height)
{
    assert(text != NULL);

    *width = strlen(text) * FontTileset.TileWidth;
    *height = FontTileset.TileHeight;
}