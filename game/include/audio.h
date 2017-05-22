#ifndef AUDIO_HEADER_INCLUDED
#define AUDIO_HEADER_INCLUDED

struct Game;

enum SfxId
{
    SFX_ATTACK_01,
    SFX_CASH_PICKUP_01,
};

int AudioInit(struct Game* game);
void AudioShutdown();
void PlaySfx(enum SfxId id);

#endif