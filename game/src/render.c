#include "actor.h"
#include "actor_list.h"
#include "flip_flag.h"
#include "game.h"
#include "map.h"
#include "panel.h"
#include "tile.h"
#include "tileset.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <string.h>

//  SDL_RenderSetScale scale amount
static const int RenderScale = 2;

static struct Tileset FontTileset;
static struct Tileset GuiTileset;
static struct Tileset MapTileset;

void DrawText(
    SDL_Renderer* renderer,
    const char* text,
    const int x,
    const int y);
void MeasureText(const char* text, int* width, int* height);

//  ---------------------------------------------------------------------------
void DrawScreenFade(SDL_Renderer* renderer, float progress)
{
    if (progress < 0)
    {
        progress = 0;
    }
    else if (progress > 1)
    {
        progress = 1;
    }

    Uint8 alpha = (int)(255 * progress);

    SDL_Rect destRect;
    SDL_RenderGetViewport(renderer, &destRect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(renderer, &destRect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void DrawMap(
    SDL_Renderer* renderer,
    const struct Map* map,
    const struct ActorList* actors)
{
    assert(renderer != NULL);
    assert(map != NULL);
    assert(MapTileset.Texture != NULL);

    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = map->TileWidth;
    destRect.h = map->TileHeight;

    for (int y = 0; y < map->Height; ++y)
    {
        for (int x = 0; x < map->Width; ++x)
        {
            int tilesetId = map->Tiles[y * map->Width + x].TilesetId;

            if (tilesetId == 0)
            {
                continue;
            }

            GetTilesetRect(&MapTileset, tilesetId, &sourceRect);

            destRect.x = x * destRect.w;
            destRect.y = y * destRect.h;
            SDL_RenderCopy(renderer, MapTileset.Texture, &sourceRect, &destRect);
        }
    }

    struct ActorListNode* actorNode = actors->Last;
    while (actorNode != NULL)
    {
        struct Actor* actor = actorNode->Actor;

        if (actor->Tile != NULL)
        {
            GetTilesetRect(&MapTileset, actor->TilesetId, &sourceRect);

            destRect.x = actor->Tile->X * destRect.w;
            destRect.y = actor->Tile->Y * destRect.h;
            SDL_RenderCopy(renderer, MapTileset.Texture, &sourceRect, &destRect);
        }

        actorNode = actorNode->Previous;
    }
}

//  ---------------------------------------------------------------------------
void DrawTilesetTile(
    SDL_Renderer* renderer,
    int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip)
{
    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = MapTileset.TileWidth;
    destRect.h = MapTileset.TileHeight;

    GetTilesetRect(&MapTileset, tilesetId, &sourceRect);

    destRect.x = x;
    destRect.y = y;

    SDL_RendererFlip flipFlags =
        (flip & FLIP_FLAG_HORZ ? SDL_FLIP_HORIZONTAL : 0) |
        (flip & FLIP_FLAG_VERT ? SDL_FLIP_VERTICAL : 0);

    SDL_RenderCopyEx(
        renderer,
        MapTileset.Texture,
        &sourceRect,
        &destRect,
        0,
        NULL,
        flipFlags);
}

//  ---------------------------------------------------------------------------
void DrawGuiTilesetTile(
    SDL_Renderer* renderer,
    const int tilesetId,
    const int x,
    const int y,
    enum FlipFlag flip)
{
    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.w = GuiTileset.TileWidth;
    destRect.h = GuiTileset.TileHeight;

    GetTilesetRect(&GuiTileset, tilesetId, &sourceRect);

    destRect.x = x;
    destRect.y = y;

    SDL_RendererFlip flipFlags =
        (flip & FLIP_FLAG_HORZ ? SDL_FLIP_HORIZONTAL : 0) |
        (flip & FLIP_FLAG_VERT ? SDL_FLIP_VERTICAL : 0);

    SDL_RenderCopyEx(
        renderer,
        GuiTileset.Texture,
        &sourceRect,
        &destRect,
        0,
        NULL,
        flipFlags);
}

//  ---------------------------------------------------------------------------
static void DrawPanelText(SDL_Renderer* renderer, const struct Panel* panel)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(panel->Text != NULL);

    int textWidth, textHeight, textX, textY;
    MeasureText(panel->Text, &textWidth, &textHeight);
    if (panel->TextAlign == PANEL_TEXT_ALIGN_LEFT)
    {
        textX = panel->X;
        textY = panel->Y;
    }
    else
    {
        textX = panel->X + (panel->Width / 2) - (textWidth / 2);
        textY = panel->Y + (panel->Height / 2) - (textHeight / 2);
    }
    DrawText(renderer, panel->Text, textX, textY);
}

//  ---------------------------------------------------------------------------
static void DrawPanelTitle(
    SDL_Renderer* renderer,
    const struct Panel* panel,
    const char* text)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(text != NULL);

    int textWidth, textHeight;
    MeasureText(text, &textWidth, &textHeight);
    int textX = panel->X + (panel->Width / 2) - (textWidth / 2);
    int textY = panel->Y - textHeight;
    DrawText(renderer, text, textX, textY);
}

//  ---------------------------------------------------------------------------
void DrawPanelBorder(SDL_Renderer* renderer, const struct Panel* panel)
{
    if (panel->BorderStyle == PANEL_BORDER_STYLE_NONE)
    {
        return;
    }

    SDL_Rect sourceRect;

    SDL_Rect destRect;
    destRect.x = panel->X;
    destRect.y = panel->Y;
    destRect.w = GuiTileset.TileWidth;
    destRect.h = GuiTileset.TileHeight;

    int cornerTilesetId, sideTilesetId;
    GetPanelBorderTilesetIds(panel->BorderStyle, &cornerTilesetId, &sideTilesetId);

    GetTilesetRect(&GuiTileset, sideTilesetId, &sourceRect);
    int startX = panel->X + destRect.w;
    int endX = panel->X + panel->Width - destRect.w;
    for (int x = startX; x < endX; x += destRect.w)
    {
        destRect.x = x;

        destRect.y = panel->Y;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            90,
            NULL,
            SDL_FLIP_NONE);

        destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            270,
            NULL,
            SDL_FLIP_NONE);
    }

    int startY = panel->Y + destRect.h;
    int endY = panel->Y + panel->Height - destRect.h;
    for (int y = startY; y < endY; y += destRect.h)
    {
        destRect.y = y;

        destRect.x = panel->X;
        SDL_RenderCopy(renderer, GuiTileset.Texture, &sourceRect, &destRect);

        destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
        SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
    }

    GetTilesetRect(&GuiTileset, cornerTilesetId, &sourceRect);

    //  Upper-left corner
    destRect.x = panel->X;
    destRect.y = panel->Y;
    SDL_RenderCopy(renderer, GuiTileset.Texture, &sourceRect, &destRect);

    //  Lower-left corner
    destRect.x = panel->X;
    destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_VERTICAL);

    //  Lower-right corner
    destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
    destRect.y = panel->Y + panel->Height - GuiTileset.TileHeight;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

    //  Upper-right corner
    destRect.x = panel->X + panel->Width - GuiTileset.TileWidth;
    destRect.y = panel->Y;
    SDL_RenderCopyEx(
            renderer,
            GuiTileset.Texture,
            &sourceRect,
            &destRect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
}

//  ---------------------------------------------------------------------------
void DrawPanel(SDL_Renderer* renderer, const struct Panel* panel)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(MapTileset.Texture != NULL);

    if (panel->Visible == 0)
    {
        return;
    }

    if (panel->ShowTitle && panel->Title != NULL)
    {
        DrawPanelTitle(renderer, panel, panel->Title);
    }

    if (panel->Background)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_Rect panelRect = { panel->X, panel->Y, panel->Width, panel->Height };
        SDL_SetRenderDrawColor(renderer, 42, 5, 3, panel->Alpha);
        SDL_RenderFillRect(renderer, &panelRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    if (panel->Alpha < 255)
    {
        SDL_SetTextureBlendMode(GuiTileset.Texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(GuiTileset.Texture, panel->Alpha);
    }

    DrawPanelBorder(renderer, panel);

    if (panel->Icon.TilesetId > -1)
    {
        if (panel->Icon.Style == PANEL_ICON_STYLE_NORMAL)
        {
            DrawTilesetTile(
                renderer,
                panel->Icon.TilesetId,
                panel->X + (panel->Width / 2) - (MapTileset.TileWidth / 2),
                panel->Y + (panel->Height / 2) - (MapTileset.TileHeight / 2),
                panel->Icon.Flip);
        }
        else if (panel->Icon.Style == PANEL_ICON_STYLE_SMALL)
        {
            DrawGuiTilesetTile(
                renderer,
                panel->Icon.TilesetId,
                panel->X + (panel->Width / 2) - (GuiTileset.TileWidth / 2),
                panel->Y + (panel->Height / 2) - (GuiTileset.TileHeight / 2),
                panel->Icon.Flip);
        }
    }

    if (panel->Text != NULL)
    {
        DrawPanelText(renderer, panel);
    }

    if (panel->Alpha < 255)
    {
        SDL_SetTextureBlendMode(GuiTileset.Texture, SDL_BLENDMODE_NONE);
        SDL_SetTextureAlphaMod(GuiTileset.Texture, 255);
    }
}

//  ---------------------------------------------------------------------------
void DrawText(
    SDL_Renderer* renderer,
    const char* text,
    const int x,
    const int y)
{
    assert(renderer != NULL);
    assert(text != NULL);
    assert(FontTileset.Texture != NULL);

    SDL_Rect destRect =
    {
        .x = x,
        .y = y,
        .w = FontTileset.TileWidth,
        .h = FontTileset.TileHeight
    };
    SDL_Rect sourceRect;

    int textLength = strlen(text);
    for (int t = 0; t < textLength; ++t)
    {
        int c = (int)text[t];
        if (c > 32 && c < 127)
        {
            GetTilesetRect(&FontTileset, c - 32, &sourceRect);
            SDL_RenderCopy(renderer, FontTileset.Texture, &sourceRect, &destRect);
        }

        destRect.x += destRect.w;
    }
}

//  ---------------------------------------------------------------------------
void DrawTextAlpha(
    SDL_Renderer* renderer,
    const char* text,
    const int x,
    const int y,
    const int alpha)
{
    SDL_SetTextureAlphaMod(FontTileset.Texture, alpha);
    DrawText(renderer, text, x, y);
    SDL_SetTextureAlphaMod(FontTileset.Texture, 255);
}

//  ---------------------------------------------------------------------------
void GetTileRect(
    const struct Map* map,
    const struct Tile* tile,
    SDL_Rect* rect,
    const int scaled)
{
    assert(map != NULL);
    assert(tile != NULL);
    assert(rect != NULL);

    int scale = scaled ? RenderScale : 1;

    (*rect).x = tile->X * map->TileWidth * scale;
    (*rect).y = tile->Y * map->TileHeight * scale;
    (*rect).w = map->TileWidth * scale;
    (*rect).h = map->TileHeight * scale;
}

//  ---------------------------------------------------------------------------
int GfxInit(struct Game* game)
{
    assert(FontTileset.Texture == NULL);
    assert(GuiTileset.Texture == NULL);
    assert(MapTileset.Texture == NULL);

    SDL_RenderSetScale(game->Renderer, RenderScale, RenderScale);

    LoadTileset(game, &FontTileset, "font");
    if (FontTileset.Texture == NULL)
    {
        printf("Failed to load font tileset texture.\n");
        return -1;
    }

    FontTileset.TileWidth = 5;
    FontTileset.TileHeight = 12;

    LoadTileset(game, &GuiTileset, "tileset");
    if (GuiTileset.Texture == NULL)
    {
        printf("Failed to load GUI tileset texture.\n");
        return -1;
    }

    GuiTileset.TileWidth = 8;
    GuiTileset.TileHeight = 8;

    LoadTileset(game, &MapTileset, "tileset");
    if (MapTileset.Texture == NULL)
    {
        printf("Failed to load map tileset texture.\n");
        return -1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void GfxShutdown()
{
    SDL_DestroyTexture(FontTileset.Texture);
    SDL_DestroyTexture(GuiTileset.Texture);
    SDL_DestroyTexture(MapTileset.Texture);
}

//  ---------------------------------------------------------------------------
void MeasureText(const char* text, int* width, int* height)
{
    assert(text != NULL);

    *width = strlen(text) * FontTileset.TileWidth;
    *height = FontTileset.TileHeight;
}