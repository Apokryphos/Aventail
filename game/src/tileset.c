#include <SDL2/SDL.h>
#include "tileset.h"
#include "game.h"
#include <assert.h>

//  ---------------------------------------------------------------------------
void GetTilesetRect(
    const struct Tileset* tileset,
    const int index,
    SDL_Rect* rect)
{
    assert(tileset->Texture != NULL);
    assert(tileset->TextureWidth > 0);
    assert(tileset->TextureHeight > 0);
    assert(tileset->TileWidth > 0);
    assert(tileset->TileHeight > 0);
    assert(rect != NULL);

    int tilesPerRow = tileset->TextureWidth / tileset->TileWidth;
    assert(tilesPerRow > 0);

    int x = index % tilesPerRow;
    int y = index / tilesPerRow;

    rect->x = x * tileset->TileWidth;
    rect->y = y * tileset->TileHeight;
    rect->w = tileset->TileWidth;
    rect->h = tileset->TileHeight;
}

//  ---------------------------------------------------------------------------
void LoadTileset(struct Game* game, struct Tileset* tileset, const char* assetPath)
{
    LoadTexture(game, &tileset->Texture, assetPath);

    tileset->TileWidth = 16;
    tileset->TileHeight = 16;

    SDL_QueryTexture(
        tileset->Texture,
        NULL,
        NULL,
        &tileset->TextureWidth,
        &tileset->TextureHeight);
}