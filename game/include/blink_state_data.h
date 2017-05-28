#ifndef BLINK_STATE_DATA_HEADER_INCLUDED
#define BLINK_STATE_DATA_HEADER_INCLUDED

struct BlinkStateData
{
    int visible;
    int enabled;
    float ticks;
    float on_duration;
    float off_duration;
};

void add_blink_state_data_time(struct BlinkStateData* blink, float elapsed_seconds);

#endif