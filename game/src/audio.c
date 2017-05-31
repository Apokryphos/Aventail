#include "audio.h"
#include "game.h"
#include "paths.h"
#include <SDL2/SDL_mixer.h>
#include <assert.h>

#define AUDIO_QUEUE_SIZE 16

static const int CHANNEL_RESERVE_COUNT = 2;
static const int STEPS_ENTER_SFX_CHANNEL = 0;
static const int STEPS_EXIT_SFX_CHANNEL = 1;

static Mix_Chunk* chunk_queue[AUDIO_QUEUE_SIZE];
static int channel_queue[AUDIO_QUEUE_SIZE];

static Mix_Chunk* attack_mix_chunk = NULL;
static Mix_Chunk* cash_pickup_mix_chunk = NULL;
static Mix_Chunk* door_mix_chunk = NULL;
static Mix_Chunk* steps_enter_mix_chunk = NULL;
static Mix_Chunk* steps_exit_mix_chunk = NULL;

//  ---------------------------------------------------------------------------
static void load_sfx(Mix_Chunk** chunk, char* asset_name)
{
    assert(chunk != NULL);
    assert(*chunk == NULL);
    assert(asset_name != NULL);

    char* full_path = create_sfx_file_path(asset_name);
    *chunk = Mix_LoadWAV(full_path);
    free(full_path);
}

//  ---------------------------------------------------------------------------
int audio_init()
{
    int init_flags = MIX_INIT_OGG;
    int init = Mix_Init(init_flags);
    if ((init & init_flags) != init_flags)
    {
        printf("Mix_Init failed to initialize OGG support.\n");
        printf("%s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        return -1;
    }

    load_sfx(&attack_mix_chunk, "attack01");
    load_sfx(&cash_pickup_mix_chunk, "cash_pickup_01");
    load_sfx(&door_mix_chunk, "door");
    load_sfx(&steps_enter_mix_chunk, "steps_enter");
    load_sfx(&steps_exit_mix_chunk, "steps_exit");

    //  Reserve a channel for stair effects
    Mix_ReserveChannels(CHANNEL_RESERVE_COUNT);

    //  Init queue
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        chunk_queue[c] = NULL;
        channel_queue[c] = -1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void shutdown_audio()
{
    //  Halt playback on all channels
    Mix_HaltChannel(-1);

    Mix_FreeChunk(attack_mix_chunk);
    Mix_FreeChunk(cash_pickup_mix_chunk);
    Mix_FreeChunk(door_mix_chunk);
    Mix_FreeChunk(steps_enter_mix_chunk);
    Mix_FreeChunk(steps_exit_mix_chunk);

    Mix_CloseAudio();
    Mix_Quit();

    //  Clear queue
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        chunk_queue[c] = NULL;
        channel_queue[c] = -1;
    }
}

//  ---------------------------------------------------------------------------
void update_audio()
{
    //  Attempt to play any queued sound effects
    for (size_t c = 0; c < AUDIO_QUEUE_SIZE; ++c)
    {
        if (chunk_queue[c] != NULL)
        {
            int played = Mix_PlayChannel(channel_queue[c], chunk_queue[c], 0);

            if (played != -1)
            {
                chunk_queue[c] = NULL;
                channel_queue[c] = -1;
            }
        }
    }
}

//  ---------------------------------------------------------------------------
void play_sfx(const enum SfxId id)
{
    //  Default is to play using any available channel
    int channel = -1;
    Mix_Chunk* chunk = NULL;
    switch (id)
    {
        case SFX_ATTACK_01:
            chunk = attack_mix_chunk;
            break;
        case SFX_CASH_PICKUP_01:
            chunk = cash_pickup_mix_chunk;
            break;
        case SFX_DOOR:
            chunk = door_mix_chunk;
            break;
        case SFX_STEPS_ENTER:
            channel = STEPS_ENTER_SFX_CHANNEL;
            chunk = steps_enter_mix_chunk;
            break;
        case SFX_STEPS_EXIT:
            channel = STEPS_EXIT_SFX_CHANNEL;
            chunk = steps_exit_mix_chunk;
            break;
    }

    assert(chunk!= NULL);

    //  Stop playing opposite steps sound effect.
    //  This is to reduce overlap of the entering and exiting steps sound
    //  effects, in case the map loads and plays the entering sound effect
    //  before the exiting sound effect finishes playing. SDL_Delay or
    //  elapsed time bookkeeping could be used but it's preferable to avoid
    //  that for now as map load times are likely to increase.
    if (channel == STEPS_ENTER_SFX_CHANNEL)
    {
        Mix_HaltChannel(STEPS_EXIT_SFX_CHANNEL);
    }
    else if (channel == STEPS_EXIT_SFX_CHANNEL)
    {
        Mix_HaltChannel(STEPS_ENTER_SFX_CHANNEL);
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
            if (chunk_queue[c] == NULL)
            {
                chunk_queue[c] = chunk;
                channel_queue[c] = channel;
                break;
            }
        }
    }
}