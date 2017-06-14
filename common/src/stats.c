#include "stats.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct Stats add_stats(const struct Stats stats1, const struct Stats stats2)
{
    struct Stats stats =
    {
        .attack = stats1.attack + stats2.attack,
        .defend = stats1.defend + stats2.defend,
        .vitality = stats1.vitality + stats2.vitality,
    };
    return stats;
}

//  ---------------------------------------------------------------------------
void clamp_stats(struct Stats* stats)
{
    if (stats->attack < 0)
    {
        stats->attack = 0;
    }

    if (stats->defend < 0)
    {
        stats->defend = 0;
    }

    if (stats->vitality < 0)
    {
        stats->vitality = 0;
    }
}

//  ---------------------------------------------------------------------------
int get_stats_value_by_stat_type(
    const struct Stats* stats,
    const enum StatType stat_type)
{
    assert(stats != NULL);
    switch (stat_type)
    {
        case STAT_TYPE_ATTACK:
            return stats->attack;
        case STAT_TYPE_DEFEND:
            return stats->defend;
        case STAT_TYPE_VITALITY:
            return stats->vitality;
        default:
            return 0;
    }
}
