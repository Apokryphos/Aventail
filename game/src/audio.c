#include "audio.h"
#include "game.h"
#include "paths.h"
#include <SDL2/SDL_mixer.h>
#include <assert.h>

#define AUDIO_QUEUE_SIZE 16

static const int ReserveChannelCount = 2;
static const int StepsEnterSfxChannel = 0;
static const int StepsExitSfxChannel = 1;

static Mix_Chunk* ChunkQueue[AUDIO_QUEUE_SIZE];
static int ChannelQueue[AUDIO_QUEUE_SIZE];

static Mix_Chunk* AttackSfxMixChunk = NULL;
static Mix_Chunk* CashPickupSfxMixChunk = NULL;
static Mix_Chunk* DoorSfxMixChunk = NULL;
static Mix_Chunk* StepsEnterSfxMixChunk = NULL;
static Mix_Chunk* StepsExitSfxMixChunk = NULL;

//  ---------------------------------------------------------------------------
static void LoadSfx(Mix_Chunk** chunk, const struct Game* game, char* assetName)
{
    assert(chunk != NULL);
    assert(*chunk == NULL);
    assert(assetName != NULL);

    char* fullPath = CreateSfxPath(game->BasePath, assetName);
    *chunk = Mix_LoadWAV(fullPath);
    free(fullPath);
}

//  ---------------------------------------------------------------------------
int AudioInit(const struct Game* game)
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
    LoadSfx(&DoorSfxMixChunk, game, "door");
    LoadSfx(&StepsEnterSfxMixChunk, game, "steps_enter");
    LoadSfx(&StepsExitSfxMixChunk, game, "steps_exit");

    //  Reserve a channel for stair effects
    Mix_ReserveChannels(ReserveChannelCount);

    //  Init queue
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        ChunkQueue[c] = NULL;
        ChannelQueue[c] = -1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void AudioShutdown()
{
    //  Halt playback on all channels
    Mix_HaltChannel(-1);

    Mix_FreeChunk(AttackSfxMixChunk);
    Mix_FreeChunk(CashPickupSfxMixChunk);
    Mix_FreeChunk(DoorSfxMixChunk);
    Mix_FreeChunk(StepsEnterSfxMixChunk);
    Mix_FreeChunk(StepsExitSfxMixChunk);

    Mix_CloseAudio();
    Mix_Quit();

    //  Clear queue 
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        ChunkQueue[c] = NULL;
        ChannelQueue[c] = -1;
    }
}

//  ---------------------------------------------------------------------------
void AudioUpdate()
{
    //  Attempt to play any queued sound effects
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        if (ChunkQueue[c] != NULL)
        {
            int played = Mix_PlayChannel(ChannelQueue[c], ChunkQueue[c], 0);

            if (played != -1)
            {
                ChunkQueue[c] = NULL;
                ChannelQueue[c] = -1;
            }
        }
    }
}

//  ---------------------------------------------------------------------------
void PlaySfx(const enum SfxId id)
{
    //  Default is to play using any available channel
    int channel = -1;
    Mix_Chunk* chunk = NULL;
    switch (id)
    {
        case SFX_ATTACK_01:
            chunk = AttackSfxMixChunk;
            break;
        case SFX_CASH_PICKUP_01:
            chunk = CashPickupSfxMixChunk;
            break;
        case SFX_DOOR:
            chunk = DoorSfxMixChunk;
            break;
        case SFX_STEPS_ENTER:
            channel = StepsEnterSfxChannel;
            chunk = StepsEnterSfxMixChunk;
            break;
        case SFX_STEPS_EXIT:
            channel = StepsExitSfxChannel;
            chunk = StepsExitSfxMixChunk;
            break;
    }

    assert(chunk!= NULL);

    //  Stop playing opposite steps sound effect.
    //  This is to reduce overlap of the entering and exiting steps sound
    //  effects, in case the map loads and plays the entering sound effect
    //  before the exiting sound effect finishes playing. SDL_Delay or
    //  elapsed time bookkeeping could be used but it's preferable to avoid
    //  that for now as map load times are likely to increase.
    if (channel == StepsEnterSfxChannel)
    {
        Mix_HaltChannel(StepsExitSfxChannel);
    }
    else if (channel == StepsExitSfxChannel)
    {
        Mix_HaltChannel(StepsEnterSfxChannel);
    }

    int played = -1;
    if (Mix_Playing(channel) == 0)
    {
        played = Mix_PlayChannel(channel, chunk, 0);
    }

    if (played == -1)
    {
        //  Add to queue if channel wasn't available
        for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
        {
            if (ChunkQueue[c] == NULL)
            {
                ChunkQueue[c] = chunk;
                ChannelQueue[c] = channel;
                break;
            }
        }     
    }
}