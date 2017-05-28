#ifndef ACTOR_LIST_HEADER_INCLUDED
#define ACTOR_LIST_HEADER_INCLUDED

struct Actor;

typedef int FindActorFunction(struct Actor* source);

struct ActorListNode
{
    struct Actor* actor;
    struct ActorListNode* previous;
    struct ActorListNode* next;
};

struct ActorList
{
    int count;
    struct ActorListNode* front;
    struct ActorListNode* back;
};

void add_actor_to_actor_list_back(struct ActorList* list, struct Actor* actor);
void add_actor_to_actor_list_front(struct ActorList* list, struct Actor* actor);
/*
    Allocates an ActorList. Free with destroy_actor_list.
*/
struct ActorList* create_actor_list();
/**
    Frees the specified ActorList, all of its ActorListNodes, and all actors
    referenced by its nodes. Afterwards the specified pointer will be set to NULL.
*/
void destroy_actor_list(struct ActorList** list);
struct Actor* find_actor_in_actor_list(const struct ActorList* list, FindActorFunction* find);
void remove_actor_from_actor_list(struct ActorList* list, const struct Actor* actor);

#endif