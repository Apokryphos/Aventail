#ifndef RENDER_HEADER_INCLUDED
#define RENDER_HEADER_INCLUDED

struct ActorList;
struct Map;
struct Tileset;
typedef struct SDL_Renderer SDL_Renderer;

void DrawMap(
    SDL_Renderer* renderer, 
    struct Map* map,
    struct ActorList* actors);
void DrawText(
    SDL_Renderer* renderer, 
    char* text,
    int x,
    int y);
int GfxInit(struct Game* game);
void GfxShutdown();

#endif