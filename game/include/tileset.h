#ifndef TILESET_HEADER_INCLUDED
#define TILESET_HEADER_INCLUDED

struct Game;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;

struct Tileset
{
    int TileWidth;
    int TileHeight;
    int TextureWidth;
    int TextureHeight;
    SDL_Texture* Texture;
};

void GetTilesetRect(
    const struct Tileset* tileset,
    const int index,
    SDL_Rect* rect);
void LoadTileset(struct Game* game, struct Tileset* tileset, const char* assetPath);

#endif