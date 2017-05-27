#ifndef RENDER_HEADER_INCLUDED
#define RENDER_HEADER_INCLUDED

#include "flip_flag.h"

struct ActorList;
struct Map;
struct Panel;
struct Tile;
struct Tileset;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Renderer SDL_Renderer;

void DrawMap(
    SDL_Renderer* renderer, 
    const struct Map* map,
    const struct ActorList* actors);
void DrawTilesetTile(
    SDL_Renderer* renderer,
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip);
void DrawGuiTilesetTile(
    SDL_Renderer* renderer,
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip);
void DrawPanel(SDL_Renderer* renderer, const struct Panel* panel);
void DrawScreenFade(SDL_Renderer* renderer, float progress);
void DrawText(
    SDL_Renderer* renderer, 
    const char* text,
    const int x,
    const int y);
void DrawTextAlpha(
    SDL_Renderer* renderer, 
    const char* text,
    const int x,
    const int y,
    const int alpha);
/*
    Gets the SDL_Rect of the specified Tile in pixels.
*/
void GetTileRect(
    const struct Map* map, 
    const struct Tile* tile, 
    SDL_Rect* rect, 
    const int scaled);
int GfxInit(struct Game* game);
void GfxShutdown();
void MeasureText(const char* text, int* width, int* height);

#endif