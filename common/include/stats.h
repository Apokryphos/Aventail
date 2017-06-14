#ifndef STATS_HEADER_INCLUDED
#define STATS_HEADER_INCLUDED

#include "stat_type.h"

struct Stats
{
    int attack;
    int defend;
    int vitality;
};

struct Stats add_stats(const struct Stats stats1, const struct Stats stats2);
/*
    Clamps stat values to positive values.
*/
void clamp_stats(struct Stats* stats);
int get_stats_value_by_stat_type(
    const struct Stats* stats,
    const enum StatType stat_type);

#endif