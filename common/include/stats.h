#ifndef STATS_HEADER_INCLUDED
#define STATS_HEADER_INCLUDED

#include "stat_type.h"

struct Stats
{
    int Attack;
    int Defend;
    int Vitality;
};

struct Stats AddStats(struct Stats stats1, struct Stats stats2);
/*
    Clamps stat values to positive values.
*/
void ClampStats(struct Stats* stats);
int GetStatByType(struct Stats* stats, enum StatType statType);

#endif