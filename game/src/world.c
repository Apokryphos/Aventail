#include "actor.h"
#include "actor_defs.h"
#include "actor_list.h"
#include "gear.h"
#include "inventory.h"
#include "item_defs.h"
#include "map.h"
#include "world.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
struct Actor* create_player_actor(struct World* world)
{
    assert(world->player.actor == NULL);

    struct Actor* actor = create_actor(NULL, "Player", -1, -1, 190, 0);
    actor->type = ACTOR_TYPE_PLAYER;
    load_player_definition(actor);
    add_actor_to_actor_list_front(world->actors, actor);
    world->player.actor = actor;

    //  Some starting inventory to test with
    struct Item* armor = create_item("Leather Cuirass");
    load_item_definition(armor);
    equip_item(actor, armor);

    // struct Item* weapon = create_item("Bronze Sword");
    // load_item_definition(weapon);
    // equip_item(actor, weapon);

    struct Item* armor2 = create_item("Chainmail");
    load_item_definition(armor2);
    add_item_to_inventory(actor->inventory, armor2);

    // struct Item* shield1 = create_item("Buckler");
    // load_item_definition(shield1);
    // add_item_to_inventory(actor->inventory, shield1);

    // add_item_to_inventory(actor->inventory, create_weapon("Dagger"));
    // add_item_to_inventory(actor->inventory, create_weapon("Dirk"));
    // add_item_to_inventory(actor->inventory, create_weapon("Knife"));
    // add_item_to_inventory(actor->inventory, create_weapon("Shiv"));
    // add_item_to_inventory(actor->inventory, create_weapon("Katana"));
    // add_item_to_inventory(actor->inventory, create_weapon("Nodachi"));
    // add_item_to_inventory(actor->inventory, create_weapon("Short Sword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Great Sword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Claymore"));
    // add_item_to_inventory(actor->inventory, create_weapon("Longsword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Broadsword"));
    // add_item_to_inventory(actor->inventory, create_weapon("Sabre"));
    // add_item_to_inventory(actor->inventory, create_weapon("Rapier"));
    // add_item_to_inventory(actor->inventory, create_weapon("Cutlass"));

    // struct Item* weapon2 = create_item("Iron Short Sword");
    // load_item_definition(weapon2);
    // add_item_to_inventory(actor->inventory, weapon2);

    struct Item* accessory1 = create_item("Healing Ring");
    load_item_definition(accessory1);
    add_item_to_inventory(actor->inventory, accessory1);

    return actor;
}

//  ---------------------------------------------------------------------------
struct World* create_world()
{
    struct World* world = malloc(sizeof(struct World));
    world->actors = create_actor_list();
    world->player.actor = NULL;
    world->map = NULL;
    return world;
}

//  ---------------------------------------------------------------------------
void destroy_world(struct World** world)
{
    if (*world == NULL)
    {
        return;
    }

    destroy_actor_list(&(*world)->actors);

    if ((*world)->map != NULL)
    {
        destroy_map(&(*world)->map);
    }

    free(*world);
    *world = NULL;
}