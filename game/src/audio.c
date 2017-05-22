#include "audio.h"
#include "game.h"
#include "paths.h"
#include <SDL2/SDL_mixer.h>
#include <assert.h>

static Mix_Chunk* AttackSfxMixChunk = NULL;
static Mix_Chunk* CashPickupSfxMixChunk = NULL;

//  ---------------------------------------------------------------------------
static void LoadSfx(Mix_Chunk** chunk, struct Game* game, char* assetName)
{
    assert(chunk != NULL);
    assert(*chunk == NULL);
    assert(assetName != NULL);

    char* fullPath = CreateSfxPath(game->BasePath, assetName);
    *chunk = Mix_LoadWAV(fullPath);
    free(fullPath);
}

//  ---------------------------------------------------------------------------
int AudioInit(struct Game* game)
{
    int initFlags = MIX_INIT_OGG;
    int init = Mix_Init(initFlags);
    if ((init & initFlags) != initFlags)
    {
        printf("Mix_Init failed to initialize OGG support.\n");
        printf("%s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        return -1;
    }

    LoadSfx(&AttackSfxMixChunk, game, "attack01");
    LoadSfx(&CashPickupSfxMixChunk, game, "cash_pickup_01");

    return 0;
}

//  ---------------------------------------------------------------------------
void AudioShutdown()
{
    //  Halt playback on all channels
    Mix_HaltChannel(-1);

    Mix_FreeChunk(AttackSfxMixChunk);
    Mix_FreeChunk(CashPickupSfxMixChunk);

    Mix_CloseAudio();
    Mix_Quit();
}

//  ---------------------------------------------------------------------------
void PlaySfx(enum SfxId id)
{
    Mix_Chunk* chunk = NULL;
    switch (id)
    {
        case SFX_ATTACK_01:
            chunk = AttackSfxMixChunk;
            break;
        case SFX_CASH_PICKUP_01:
            chunk = CashPickupSfxMixChunk;
            break;
    }

    assert(chunk!= NULL);
    Mix_PlayChannel(-1, chunk, 0);
}