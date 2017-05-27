#include "blink_state_data.h"

//  ---------------------------------------------------------------------------
void AddTimeBlinkStateData(struct BlinkStateData* blink, float elapsedSeconds)
{
    blink->Ticks += elapsedSeconds;

    float duration =
        blink->Visible ?
        blink->OnDuration :
        blink->OffDuration;

    if (blink->Ticks >= duration)
    {
        blink->Ticks -= duration;

        if (blink->Enabled)
        {
            blink->Visible = !blink->Visible;
        }
    }
}