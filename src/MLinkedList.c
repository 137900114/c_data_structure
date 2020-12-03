#include "MLinkedList.h"
#include "MMemoryPool.h"
#include "MMemoryBuffer.h"

#include <string.h>

static size_t linked_list_node_size(size_t element_size){
    return element_size + sizeof(MLinkedListNode);
}

static MLinkedListNode* make_node(size_t element_size,void* data,constructor_copy_t copy){
    size_t node_size = linked_list_node_size(element_size);

    MLinkedListNode* newnode = (MLinkedListNode*)mempool_allocate(node_size);
    newnode->next = NULL;
    newnode->value = (char*)newnode + sizeof(MLinkedListNode);

    copy(newnode->value,data,element_size);
    return newnode;
}

static void destroy_node(MLinkedListNode* node,size_t element_size,deconstructor_t deco){
    deco(node->value);
    size_t size = linked_list_node_size(element_size);

    mempool_deallocate(node,size);
}


MLinkedList mlist_make(size_t element_size){
    
    MLinkedList list;
    list.element = default_container_element_desc(element_size);
    list.end = NULL;
    list.head = NULL;
    list.length = 0;

    return list;
}

void mlist_clear(MLinkedList* list){
    MLinkedListNode* node = list->head;

    while(node != NULL){
        MLinkedListNode* next = node->next;
        destroy_node(node,list->element.element_size,list->element.decons);
        node = next;
    }

    list->end = NULL;
    list->head = NULL;
    list->length = 0;

    return;
}

void mlist_destroy(MLinkedList* list){
    mlist_clear(list);

    list->element = default_container_element_desc(0);
    return;
}

BOOL mlist_insert(MLinkedList* list,size_t index,void* data){
    if(index >= list->length){
        if(index != 0) return FALSE;
    }
    MLinkedListNode* newnode = make_node(list->element.element_size,data,list->element.cons_copy);
    
    if(list->length == 0){
        list->end = newnode;
        list->head = newnode;
        list->length = 1;
        return TRUE;
    }

    if(index == list->length - 1){
        list->end->next = newnode;
        list->end = newnode;

        list->length++;
        return TRUE;
    }
    MLinkedListNode* curr = list->head;
    for(int i = 0;i < index;i++){
        curr = curr->next;
    }

    newnode->next = curr->next;
    curr->next = newnode;

    list->length++;
    return TRUE;
}

BOOL mlist_insert_head(MLinkedList* list,void* data){
    MLinkedListNode* node = make_node(list->element.element_size,data,list->element.cons_copy);

    if(list->length == 0){
        list->head = node;
        list->end = node;
        list->length++;
        return TRUE;
    }

    node->next = list->head;
    list->head = node;
    list->length++;
    return TRUE;
}

BOOL mlist_insert_end(MLinkedList* list,void* data){
    if(list->length == 0)
        return mlist_insert_head(list,data);
    return mlist_insert(list,list->length - 1,data);
}

BOOL mlist_remove(MLinkedList* list,size_t index){
    if(index >= list->length){
        return FALSE;
    }

    MLinkedListNode* target;
    
    if(index == 0){
        target = list->head;
        list->head = list->head->next;
    }else{
        MLinkedListNode* curr = list->head;
        for(int i = 0;i < index - 1;i++){
            curr = curr->next;
        }
        target = curr->next;
        curr->next = target->next;
    }

    destroy_node(target,list->element.element_size,list->element.decons);
    list->length--;
    return TRUE;
}


BOOL mlist_remove_head(MLinkedList* list){
    if(list->head == NULL){
        return FALSE;
    }

    MLinkedListNode* node = list->head;
    list->head = node->next;
    list->length--;

    destroy_node(node,list->element.element_size,list->element.decons);
    return TRUE;
}

BOOL mlist_remove_end(MLinkedList* list){
    return mlist_remove(list,list->length - 1);
}

size_t mlist_size(MLinkedList* list){
    return list->length;
}

BOOL mlist_visit(MLinkedList* list,size_t index,void* data){
    if(index >= list->length){
        return FALSE;
    }

    MLinkedListNode* target = list->head;
    for(int i = 0;i != index;i++){
        target = target->next;
    }

    list->element.cons_copy(data,target->value,list->element.element_size);
    return TRUE;
}

BOOL mlist_set(MLinkedList* list,size_t index,void* data){
    MLinkedListNode* node = mlist_get_node(list,index);
    if(node == NULL) return FALSE;
    list->element.cons_copy(node->value,data,list->element.element_size);
    return TRUE;
}

inline BOOL mlist_set_string(MLinkedList* list,size_t index,const char* str){
    MString string = make_string(str);
    BOOL rv = mlist_set(list,index,&string);
    destroy_string(&string);
    return rv;
}

MLinkedListNode* mlist_get_head(MLinkedList* list){
    return list->head;
}

MLinkedListNode* mlist_get_node(MLinkedList* list,size_t index){
    if(list->length <= index) return NULL;
    MLinkedListNode* node = list->head;
    while(--index != 0){
        node = node->next;
    } 
    return node;
}