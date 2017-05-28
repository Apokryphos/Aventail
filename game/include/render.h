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

void draw_map(
    SDL_Renderer* renderer,
    const struct Map* map,
    const struct ActorList* actors);
void draw_tileset_tile(
    SDL_Renderer* renderer,
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip);
void draw_gui_tileset_tile(
    SDL_Renderer* renderer,
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip);
void draw_panel(SDL_Renderer* renderer, const struct Panel* panel);
void draw_screen_fade(SDL_Renderer* renderer, float progress);
void draw_text(
    SDL_Renderer* renderer,
    const char* text,
    const int x,
    const int y);
void draw_alpha_text(
    SDL_Renderer* renderer,
    const char* text,
    const int x,
    const int y,
    const int alpha);
/*
    Gets the SDL_Rect of the specified Tile in pixels.
*/
void get_tile_rect(
    const struct Map* map,
    const struct Tile* tile,
    SDL_Rect* rect,
    const int scaled);
int init_gfx(struct Game* game);
void shutdown_gfx();
void measure_text(const char* text, int* width, int* height);

#endif