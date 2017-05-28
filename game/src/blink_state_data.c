#include "blink_state_data.h"

//  ---------------------------------------------------------------------------
void add_blink_state_data_time(
    struct BlinkStateData* blink,
    float elapsed_seconds)
{
    blink->ticks += elapsed_seconds;

    float duration =
        blink->visible ?
        blink->on_duration :
        blink->off_duration;

    if (blink->ticks >= duration)
    {
        blink->ticks -= duration;

        if (blink->enabled)
        {
            blink->visible = !blink->visible;
        }
    }
}