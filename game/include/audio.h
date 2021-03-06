#ifndef AUDIO_HEADER_INCLUDED
#define AUDIO_HEADER_INCLUDED

struct Game;

enum SfxId
{
    SFX_ATTACK_01,
    SFX_CASH_PICKUP_01,
    SFX_DOOR,
    SFX_ERROR,
    SFX_HEAL,
    SFX_MENU_NAV,
    SFX_STEPS_ENTER,
    SFX_STEPS_EXIT,
};

int audio_init();
void shutdown_audio();
void update_audio();
void play_sfx(const enum SfxId id);

#endif