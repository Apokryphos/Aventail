#include "actor.h"
#include "actor_ai.h"
#include "map.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct ActorAi* create_actor_ai()
{
    struct ActorAi* actor_ai = malloc(sizeof(struct ActorAi));

    actor_ai->target = NULL;

    return actor_ai;
}

//  ---------------------------------------------------------------------------
void destroy_actor_ai(struct ActorAi** actor_ai)
{
    if (*actor_ai != NULL)
    {
        free(*actor_ai);
        *actor_ai = NULL;
    }
}