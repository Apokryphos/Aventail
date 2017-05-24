#ifndef ACTOR_LIST_HEADER_INCLUDED
#define ACTOR_LIST_HEADER_INCLUDED

struct Actor;

typedef int FindActorFunction(struct Actor* source);

struct ActorListNode
{
    struct Actor* Actor;
    struct ActorListNode* Previous;
    struct ActorListNode* Next;
};

struct ActorList
{
    int Count;
    struct ActorListNode* First;
    struct ActorListNode* Last;
};

void AddActorToBack(struct ActorList* list, struct Actor* actor);
void AddActorToFront(struct ActorList* list, struct Actor* actor);
/*
    Allocates an ActorList. Free with DestroyActorList.
*/
struct ActorList* CreateActorList();
/**
    Frees the specified ActorList, all of its ActorListNodes, and all actors
    referenced by its nodes. Afterwards the specified pointer will be set to NULL.
*/
void DestroyActorList(struct ActorList** list);
struct Actor* FindActor(const struct ActorList* list, FindActorFunction* find);
void RemoveActor(struct ActorList* list, const struct Actor* actor);

#endif