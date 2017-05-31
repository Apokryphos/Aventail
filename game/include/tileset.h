#ifndef TILESET_HEADER_INCLUDED
#define TILESET_HEADER_INCLUDED

struct Game;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;

struct Tileset
{
    int tile_width;
    int tile_height;
    int texture_width;
    int texture_height;
    SDL_Texture* texture;
};

void get_tileset_rect(
    const struct Tileset* tileset,
    const int index,
    SDL_Rect* rect);

void load_tileset(struct Tileset* tileset, const char* asset_path);

#endif