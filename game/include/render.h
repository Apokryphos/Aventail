#ifndef RENDER_HEADER_INCLUDED
#define RENDER_HEADER_INCLUDED

#include "flip_flag.h"

struct Actor;
struct ActorList;
struct Map;
struct Panel;
struct Tile;
struct Tileset;
struct World;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Window SDL_Window;

void draw_map(
    const struct Map* map,
    struct ActorList* actors);

void draw_tileset_tile(
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip);

void draw_panel(const struct Panel* panel);

void draw_screen_fade(float progress);

void draw_text(
    const char* text,
    const int x,
    const int y);

void draw_text_centered(
    const char* text,
    const int x,
    const int y);

void draw_alpha_text(
    const char* text,
    const int x,
    const int y,
    const int alpha);

/*
    Gets an SDL_Rect of the bounds of the active renderer viewport.
*/
void get_viewport(SDL_Rect* rect);

void get_viewport_center(int* x, int *y);

/*
    Gets the SDL_Rect of the specified Tile in pixels.
*/
void get_tile_rect(
    const struct Map* map,
    const struct Tile* tile,
    SDL_Rect* rect,
    const int scaled);

void load_texture(SDL_Texture **texture, const char* asset_filename);

void measure_text(const char* text, int* width, int* height);

/*
*   Clears the display.
*/
void render_clear();

int render_init(SDL_Window* window);

void render_shutdown();

/*
*   Updates the display by swapping the back-buffer.
*/
void render_swap();

#endif