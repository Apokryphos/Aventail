#include "game.h"
#include "actor.h"
#include "game_state.h"
#include "game_state_level.h"
#include "input.h"
#include "map.h"
#include "map_link.h"
#include "map_util.h"
#include "paths.h"
#include "render.h"
#include "tileset.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void LoadMap(
    struct Game* game, 
    const char* assetFilename,
    struct Map** map,
    struct ActorList* actors)
{
    assert(*map == NULL);
    assert(assetFilename != NULL);

    char *fullpath = CreateMapPath(game->BasePath, assetFilename);
    printf("%s\n", fullpath);
    FILE *file = fopen(fullpath, "rb");
    assert(file != NULL);
    free(fullpath);
    *map = LoadMapFromFile(file);
    LoadActorsFromFile(file, *map, actors);
    fclose(file);
}

//  ---------------------------------------------------------------------------
void LoadMapLink(struct Game* game)
{
    struct MapLink* link = game->World->LoadMapLink;

    assert(link != NULL);
    assert(link->DestMap != NULL);

    game->World->LoadMapLink = NULL;
    
    //  Copy destination link data before DestroyMap frees it
    char* destMap = strdup(link->DestMap);
    int destX = link->DestX;
    int destY = link->DestY;

    struct World* world = game->World;

    //  Remove player from actors so it doesn't get freed
    RemoveActor(world->Actors, world->Player.Actor);

    DestroyActorList(&world->Actors);
    DestroyMap(&world->Map);

    game->World->Actors = CreateActorList();

    //  Add player actor back
    AddActor(world->Actors, world->Player.Actor);

    LoadMap(game, destMap, &world->Map, world->Actors);

    free(destMap);

    struct Tile* destTile = GetTile(world->Map, destX, destY);
    assert(destTile != NULL);
    assert(InBounds(world->Map, destX, destY));

    world->Player.Actor->Map = world->Map;
    world->Player.Actor->Tile = destTile;
}

//  ---------------------------------------------------------------------------
void GameInit(struct Game* game, int width, int height)
{
    assert(game != NULL);
    assert(width > 0);
    assert(height > 0);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
		printf("SDL failed to initialize: %sn", SDL_GetError());
		exit(1);
	}

    game->Window = SDL_CreateWindow(
        "Game", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (game->Window == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    game->Renderer = SDL_CreateRenderer(game->Window, -1, SDL_RENDERER_ACCELERATED);

    if (game->Renderer == NULL)
    {
		printf("SDL failed to create window: %sn", SDL_GetError());
		exit(1);
    }

    //  Clear window
    SDL_SetRenderDrawColor(game->Renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->Renderer);
    SDL_RenderPresent(game->Renderer);

    game->BasePath = SDL_GetBasePath();
    if (game->BasePath == NULL)
    {
        game->BasePath = SDL_strdup("./");
    }

    game->ElapsedSeconds = 0;
    game->Quit = 0;
    game->State = GAME_STATE_LEVEL;

    IMG_Init(IMG_INIT_PNG);
}

//  ---------------------------------------------------------------------------
void GameMain()
{
    struct Game game = {0};

    GameInit(&game, 800, 800);

    game.World = CreateWorld();
    LoadMap(&game, "map01", &game.World->Map, game.World->Actors);

    struct InputDevice inputDevice = {0};
    game.InputDevice = &inputDevice;

    struct Input input;

    CreatePlayerActor(game.World);

    struct Tileset tileset;
    LoadTileset(&game, &tileset, "tileset");
    
    if (tileset.Texture == NULL)
    {
        printf("Failed to load tileset texture.\n");
        exit(1);
    }

    unsigned int lastTicks = 0;
    unsigned int ticks = 0;
    while (game.Quit == 0)
    {
        lastTicks = ticks;
        ticks = SDL_GetTicks();
        game.ElapsedSeconds = (ticks - lastTicks) / 1000.0f;

        UpdateInput(&input, game.InputDevice);
        game.Quit = game.InputDevice->Quit;
        
        switch (game.State)
        {
            case GAME_STATE_LEVEL:
                LevelMain(&game);
                break;
        }

        if (game.World->LoadMapLink != NULL)
        {
            LoadMapLink(&game);
        }

        SDL_RenderClear(game.Renderer);
        DrawMap(game.Renderer, game.World->Map, game.World->Actors, &tileset);
        SDL_RenderPresent(game.Renderer);
    }

    SDL_DestroyTexture(tileset.Texture);

    GameShutdown(&game);
}

//  ---------------------------------------------------------------------------
void GameShutdown(struct Game* game)
{
    if (game->BasePath != NULL)
    {
        SDL_free(game->BasePath);
        game->BasePath = NULL;
    }

    if (game->World != NULL)
    {
        DestroyWorld(&game->World);
        game->World = NULL;
    }

    if (game->Renderer != NULL)
    {
        SDL_DestroyRenderer(game->Renderer);
        game->Renderer = NULL;
    }

    if (game->Window != NULL)
    {
        SDL_DestroyWindow(game->Window);
        game->Window = NULL;
    }

    IMG_Quit();
    SDL_Quit();
}

//  ---------------------------------------------------------------------------
void LoadTexture(struct Game* game, SDL_Texture **texture, const char* assetFilename)
{
    char *fullpath = CreateTexturePath(game->BasePath, assetFilename);
    *texture = IMG_LoadTexture(game->Renderer, fullpath);
    free(fullpath);
}