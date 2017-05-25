#include "stats.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct Stats AddStats(struct Stats stats1, struct Stats stats2)
{
    struct Stats stats = 
    {
        .Attack = stats1.Attack + stats2.Attack,
        .Defend = stats1.Defend + stats2.Defend,
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
}