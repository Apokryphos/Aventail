#include "stats.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct Stats AddStats(struct Stats stats1, struct Stats stats2)
{
    struct Stats stats = 
    {
        .Attack = stats1.Attack + stats2.Attack,
        .Defend = stats1.Defend + stats2.Defend,
        .Vitality = stats1.Vitality + stats2.Vitality,
    };
    return stats;
}

//  ---------------------------------------------------------------------------
void ClampStats(struct Stats* stats)
{
    if (stats->Attack < 0)
    {
        stats->Attack = 0;
    }

    if (stats->Defend < 0)
    {
        stats->Defend = 0;
    }

    if (stats->Vitality < 0)
    {
        stats->Vitality = 0;
    }
}

//  ---------------------------------------------------------------------------
int GetStatByType(struct Stats* stats, enum StatType statType)
{
    assert(stats != NULL);
    switch (statType)
    {
        case STAT_TYPE_ATTACK:
            return stats->Attack;
        case STAT_TYPE_DEFEND:
            return stats->Defend;
        case STAT_TYPE_VITALITY:
            return stats->Vitality;
        default:
            return 0;
    }
}
