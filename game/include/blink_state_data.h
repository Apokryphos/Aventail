#ifndef BLINK_STATE_DATA_HEADER_INCLUDED
#define BLINK_STATE_DATA_HEADER_INCLUDED

struct BlinkStateData
{
    int Visible;
    int Enabled;
    float Ticks;
    float OnDuration;
    float OffDuration;
};

void AddTimeBlinkStateData(struct BlinkStateData* blink, float elapsedSeconds);

#endif