#ifndef _MLinkedList_
#define _MLinkedList_

#include "datadef.h"

typedef struct _MLinkedListNode MLinkedListNode;

typedef struct _MLinkedListNode{
    MLinkedListNode* next;
    void* value;
} MLinkedListNode;


typedef struct _MLinkedList{
    MLinkedListNode* head;
    MLinkedListNode* end;
    size_t element_size;
    size_t length;
} MLinkedList;


MLinkedList mlist_make(size_t element_size);
void mlist_destroy(MLinkedList* list);

//insert a node in the back of a index
BOOL mlist_insert(MLinkedList* list,size_t index,void* data);
//insert a node before every node
BOOL mlist_insert_head(MLinkedList* list,void* data);
BOOL mlist_insert_end(MLinkedList* list,void* data);

BOOL mlist_remove(MLinkedList* list,size_t index);
BOOL mlist_remove_head(MLinkedList* list);
BOOL mlist_remove_end(MLinkedList* list);

size_t mlist_size(MLinkedList* list);

BOOL mlist_visit(MLinkedList* list,size_t index,void* data);

MLinkedListNode* mlist_get_head(MLinkedList* list);

#endif