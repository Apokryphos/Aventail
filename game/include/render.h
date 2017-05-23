#ifndef RENDER_HEADER_INCLUDED
#define RENDER_HEADER_INCLUDED

struct ActorList;
struct Map;
struct Panel;
struct Tile;
struct Tileset;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Renderer SDL_Renderer;

void DrawMap(
    SDL_Renderer* renderer, 
    struct Map* map,
    struct ActorList* actors);
void DrawPanel(SDL_Renderer* renderer, struct Panel* panel);
void DrawText(
    SDL_Renderer* renderer, 
    const char* text,
    int x,
    int y);
void DrawTextAlpha(
    SDL_Renderer* renderer, 
    const char* text,
    int x,
    int y,
    int alpha);
/*
    Gets the SDL_Rect of the specified Tile in pixels.
*/
void GetTileRect(struct Map* map, struct Tile* tile, SDL_Rect* rect);
int GfxInit(struct Game* game);
void GfxShutdown();
void MeasureText(const char* text, int* width, int* height);

#endif