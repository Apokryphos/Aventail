#include "actor.h"
#include "actor_list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//  ---------------------------------------------------------------------------
void AddActor(struct ActorList* list, struct Actor* actor)
{
    assert(list != NULL);
    assert(actor != NULL);

    struct ActorListNode* node = malloc(sizeof(struct ActorListNode));
    node->Actor = actor;
    node->Previous = NULL;
    node->Next = NULL;

    if (list->First == NULL)
    {  
        assert(list->Count == 0);

        list->First = node;
        list->Count = 1;
    }
    else
    {
        struct ActorListNode* n = list->First;
        while (n->Next != NULL)
        {
            assert(n->Actor != actor);
            n = n->Next;
        }
        n->Next = node;
        node->Previous = n;
        ++list->Count;
    }

    list->Last = node;
}

//  ---------------------------------------------------------------------------
void AddActorToFront(struct ActorList* list, struct Actor* actor)
{
    assert(list != NULL);
    assert(actor != NULL);

    if (list->First == NULL)
    {
        AddActor(list, actor);        
    }
    else
    {
        struct ActorListNode* node = malloc(sizeof(struct ActorListNode));
        node->Actor = actor;
        node->Previous = NULL;
        node->Next = list->First;
        list->First->Previous = node;
        list->First = node;
        ++list->Count;
    }
}

//  ---------------------------------------------------------------------------
void ClearActorList(struct ActorList* list)
{
    assert(list != NULL);

    struct ActorListNode* node = list->First;
    while (node != NULL)
    {
        struct ActorListNode* next = node->Next;
        free(node);
        node = next;
    }

    list->Count = 0;
    list->First = NULL;
    list->Last = NULL;
}

//  ---------------------------------------------------------------------------
struct ActorList* CreateActorList()
{
    struct ActorList* actors =  malloc(sizeof(struct ActorList));
    actors->Count = 0;
    actors->First = NULL;
    actors->Last = NULL;
    return actors;
}

//  ---------------------------------------------------------------------------
void DestroyActorList(struct ActorList** list)
{
    if (*list == NULL)
    {
        return;
    }

    struct ActorListNode* node = (*list)->First;
    while (node != NULL)
    {
        struct ActorListNode* next = node->Next;
        DestroyActor(&node->Actor);
        free(node);
        node = next;
    }

    free(*list);
    *list = NULL;
}

//  ---------------------------------------------------------------------------
struct Actor* FindActor(struct ActorList* list, FindActorFunction* find)
{
    assert(list != NULL);
    assert(list->Count > 0);
    assert(find != NULL);

    struct ActorListNode* node = list->First;
    while (node != NULL)
    {
        if ((*find)(node->Actor))
        {
            return node->Actor;
        }

        node = node->Next;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
void RemoveActor(struct ActorList* list, struct Actor* actor)
{
    assert(list != NULL);
    assert(list->Count > 0);
    assert(actor != NULL);

    struct ActorListNode* node = list->First;
    while (node != NULL)
    {
        if (node->Actor == actor)
        {
            if (node == list->First)
            {
                list->First = list->First->Next;
            }

            if (node == list->Last)
            {
                list->Last = list->Last->Previous;
            }

            if (node->Previous != NULL)
            {
                node->Previous->Next = node->Next;
            }

            if (node->Next != NULL)
            {
                node->Next->Previous = node->Previous;
            }

            --list->Count;
            free(node);
            return;
        }

        node = node->Next;
    }
}