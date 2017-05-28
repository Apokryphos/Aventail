#include <SDL2/SDL.h>
#include "tileset.h"
#include "game.h"
#include <assert.h>

//  ---------------------------------------------------------------------------
void get_tileset_rect(
    const struct Tileset* tileset,
    const int index,
    SDL_Rect* rect)
{
    assert(tileset->texture != NULL);
    assert(tileset->texture_width > 0);
    assert(tileset->texture_height > 0);
    assert(tileset->tile_width > 0);
    assert(tileset->tile_height > 0);
    assert(rect != NULL);

    int per_row = tileset->texture_width / tileset->tile_width;
    assert(per_row > 0);

    int x = index % per_row;
    int y = index / per_row;

    rect->x = x * tileset->tile_width;
    rect->y = y * tileset->tile_height;
    rect->w = tileset->tile_width;
    rect->h = tileset->tile_height;
}

//  ---------------------------------------------------------------------------
void load_tileset(
    struct Game* game,
    struct Tileset* tileset,
    const char* asset_path)
{
    load_texture(game, &tileset->texture, asset_path);

    tileset->tile_width = 16;
    tileset->tile_height = 16;

    SDL_QueryTexture(
        tileset->texture,
        NULL,
        NULL,
        &tileset->texture_width,
        &tileset->texture_height);
}