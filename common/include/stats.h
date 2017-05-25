#ifndef STATS_HEADER_INCLUDED
#define STATS_HEADER_INCLUDED

struct Stats
{
    int Attack;
    int Defend;
};

struct Stats AddStats(struct Stats stats1, struct Stats stats2);
/*
    Clamps stat values to positive values.
*/
void ClampStats(struct Stats* stats);

#endif