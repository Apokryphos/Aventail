#include "actor.h"
#include "actor_list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//  ---------------------------------------------------------------------------
void add_actor_to_actor_list_back(struct ActorList* list, struct Actor* actor)
{
    assert(list != NULL);
    assert(actor != NULL);

    struct ActorListNode* node = malloc(sizeof(struct ActorListNode));
    node->actor = actor;
    node->previous = NULL;
    node->next = NULL;

    if (list->front == NULL)
    {
        assert(list->count == 0);

        list->front = node;
        list->count = 1;
    }
    else
    {
        struct ActorListNode* n = list->front;
        while (n->next != NULL)
        {
            assert(n->actor != actor);
            n = n->next;
        }
        n->next = node;
        node->previous = n;
        ++list->count;
    }

    list->back = node;
}

//  ---------------------------------------------------------------------------
void add_actor_to_actor_list_front(struct ActorList* list, struct Actor* actor)
{
    assert(list != NULL);
    assert(actor != NULL);

    if (list->front == NULL)
    {
        add_actor_to_actor_list_back(list, actor);
    }
    else
    {
        struct ActorListNode* node = malloc(sizeof(struct ActorListNode));
        node->actor = actor;
        node->previous = NULL;
        node->next = list->front;
        list->front->previous = node;
        list->front = node;
        ++list->count;
    }
}

//  ---------------------------------------------------------------------------
void ClearActorList(struct ActorList* list)
{
    assert(list != NULL);

    struct ActorListNode* node = list->front;
    while (node != NULL)
    {
        struct ActorListNode* next = node->next;
        free(node);
        node = next;
    }

    list->count = 0;
    list->front = NULL;
    list->back = NULL;
}

//  ---------------------------------------------------------------------------
struct ActorList* create_actor_list()
{
    struct ActorList* list = malloc(sizeof(struct ActorList));
    list->count = 0;
    list->front = NULL;
    list->back = NULL;
    return list;
}

//  ---------------------------------------------------------------------------
void destroy_actor_list(struct ActorList** list)
{
    if (*list == NULL)
    {
        return;
    }

    struct ActorListNode* node = (*list)->front;
    while (node != NULL)
    {
        struct ActorListNode* next = node->next;
        destroy_actor(&node->actor);
        free(node);
        node = next;
    }

    free(*list);
    *list = NULL;
}

//  ---------------------------------------------------------------------------
struct Actor* find_actor_in_actor_list(
    const struct ActorList* list,
    FindActorFunction* find)
{
    assert(list != NULL);
    assert(list->count > 0);
    assert(find != NULL);

    struct ActorListNode* node = list->front;
    while (node != NULL)
    {
        if ((*find)(node->actor))
        {
            return node->actor;
        }

        node = node->next;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
void remove_actor_from_actor_list(
    struct ActorList* list,
    const struct Actor* actor)
{
    assert(list != NULL);
    assert(list->count > 0);
    assert(actor != NULL);

    struct ActorListNode* node = list->front;
    while (node != NULL)
    {
        if (node->actor == actor)
        {
            if (node == list->front)
            {
                list->front = list->front->next;
            }

            if (node == list->back)
            {
                list->back = list->back->previous;
            }

            if (node->previous != NULL)
            {
                node->previous->next = node->next;
            }

            if (node->next != NULL)
            {
                node->next->previous = node->previous;
            }

            --list->count;
            free(node);
            return;
        }

        node = node->next;
    }
}

//  ---------------------------------------------------------------------------
static void swap_actor_node(
    struct ActorListNode* node1,
    struct ActorListNode* node2)
{
    struct Actor* tmp = node1->actor;
    node1->actor = node2->actor;
    node2->actor = tmp;
}

//  ---------------------------------------------------------------------------
void sort_actor_list(struct ActorList* actors)
{
    if (actors->count == 0)
    {
        return;
    }

    //  Set initial draw order values
    struct ActorListNode* node1 = actors->front;
    while (node1 != NULL)
    {
        node1->actor->draw_order = get_actor_draw_order(node1->actor);
        node1 = node1->next;
    }

    int swapped = 1;
    struct ActorListNode* node2 = NULL;
    while (swapped)
    {
        swapped = 0;
        node1 = actors->front;

        while (node1->next != node2)
        {
            assert(node1 != NULL);
            assert(node1->next != NULL);
            if (node1->actor->draw_order > node1->next->actor->draw_order)
            {
                swap_actor_node(node1, node1->next);
                swapped = 1;
            }
            node1 = node1->next;
        }
        node2 = node1;
    }
}