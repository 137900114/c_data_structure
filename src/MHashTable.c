#include "MHashTable.h"
#include "MMemoryBuffer.h"
#include "MLinkedList.h"
#include <string.h>
/*
typedef size_t (* mhash_function_t)(void* data,size_t table_size);
typedef BOOL (* mhash_key_comparator)(void* dest,void* src);
*/

#ifndef REINTERPRET_CAST_PTR
#define REINTERPRET_CAST_PTR(type,ptr) *(type *)(ptr)
#endif

static size_t int_float_hash_function(void* data,size_t table_size){
    size_t key = REINTERPRET_CAST_PTR(unsigned int,data);
    return key % table_size;
}

static size_t ll_double_hash_function(void* data,size_t  table_size){
    size_t key = REINTERPRET_CAST_PTR(size_t,data);
    return key % table_size;
}

static size_t string_hash_function(void* data,size_t table_size){
    MString* key = (MString*)data;
    size_t key_str_len = string_len(key);
    char* key_str = string_data(key);

    size_t key_index = 0;
    for(size_t i = 0;i != key_str_len;i++){
        key_index += 5 * key_index + key_str[i];
    }
    return key_index % table_size;
}

static BOOL int_float_comparator(void* lhs,void* rhs){
    return memcmp(lhs,rhs,sizeof(int)) == 0;
}

static BOOL ll_comparator(void* lhs,void* rhs){
    return memcmp(lhs,rhs,sizeof(long long));
}

static BOOL string_comparator(void* lhs,void* rhs){
    return memcmp(((MString*)lhs)->data,((MString*)rhs)->data,((MString*)lhs)->size) == 0;
}

#define hash_table_prime_list_size 28
static size_t hash_table_prime_list[hash_table_prime_list_size] = {
    53,97,193,389,769,1543,3079,6151,12289,24593,49157,
    98317,196613,393241,786433,1572869,3145739,6291469,
    12582917,25165843,50331653,100663319,201326611,
    402653189,805306457,1610612741,3221225473ul,
    4294967291ul
};

static size_t find_next_hash_table_size(size_t size){
    for(int i = 0;i != hash_table_prime_list_size;i++){
        if(hash_table_prime_list[i] > size){
            return hash_table_prime_list[i];
        }
    }
    return size * 2 - 1;
}


static MHashTable mhash_table_make_with_bucket(size_t key_element_size,size_t value_element_size,
            mhash_function_t hasher,mhash_key_comparator comparator,size_t bucket){
    MHashTable table;
    table.table_bucket_size = bucket;
    table.key_element = default_container_element_desc(key_element_size);
    table.value_element = default_container_element_desc(value_element_size);

    table.hasher = hasher;
    table.comparator = comparator;

    table.hashtable = marray_make_with_size(sizeof(MLinkedList),table.table_bucket_size);
    for(size_t i = 0;i != table.table_bucket_size; i++){
        MLinkedList* lList = marray_data(&table.hashtable,i);
        *lList = mlist_make(key_element_size + value_element_size);
    }
    table.table_size = 0;

    return table;
}

//try to resize the hashtable.If we need rehash the table will be resized,otherwise nothing will happen
static void mhash_table_try_to_resize(MHashTable* table,size_t newsize){
    if(table->table_bucket_size > newsize){
        return;
    }
    
    size_t new_table_bucket_size = find_next_hash_table_size(newsize);
    MArray new_table = marray_make_with_size(sizeof(MLinkedList),new_table_bucket_size);
    size_t new_table_element_size = table->key_element.element_size + table->value_element.element_size;
    for(size_t i  = 0;i != new_table_bucket_size;i++){
        MLinkedList* new_list = (MLinkedList*)marray_data(&new_table,i);
        *new_list = mlist_make(new_table_element_size);
    }

    for(size_t i = 0;i != table->table_bucket_size;i++){
        MLinkedList* old_list = (MLinkedList*)marray_data(&table->hashtable,i);
        MLinkedListNode* old_list_node = old_list->head;

        while(old_list_node != NULL){
            MLinkedListNode* target_node = old_list_node;
            old_list_node = old_list_node->next;

            size_t hash_key = table->hasher(old_list_node->value,new_table_bucket_size);
            MLinkedList* new_list = (MLinkedList*)marray_data(&new_table,hash_key);
            if(new_list->head == NULL){
                new_list->head = target_node;
                new_list->end = target_node;
            }else{
                new_list->end->next = target_node;
                new_list->end = target_node;
            }
            target_node->next = NULL;
            new_list->length++;
        }

        old_list->head = NULL,old_list->end = NULL;
        old_list->length = 0;
    }

    marray_destroy(&table->hashtable);
    table->hashtable = new_table;
    table->table_bucket_size = new_table_bucket_size;
}

//we don't allowed the same keys are maintained in the table.
static BOOL mhash_table_insert_after_resize(MHashTable* table,void* key_element,void* value_element){
    size_t hash_key = table->hasher(key_element,table->table_bucket_size);
    MLinkedList* target_bucket = (MLinkedList*)marray_data(&table->hashtable,hash_key);

    for(MLinkedListNode* curr = mlist_get_head(target_bucket);curr != NULL;curr = curr->next){
        //the same key shouldn't appear in the table twice
        if(table->comparator(curr->value,key_element)){
            return FALSE;
        }
    }

    mlist_insert_end(target_bucket,NULL);
    MLinkedListNode* end = mlist_get_end(target_bucket);
    table->key_element.cons_copy(end->value,key_element,table->key_element.element_size);
    table->value_element.cons_copy(((char*)end->value) + table->key_element.element_size,
            value_element,table->value_element.element_size);

    return TRUE;
}

inline BOOL mhash_table_insert(MHashTable* table,void* key_element,void* value_element){
    mhash_table_try_to_resize(table,table->table_size + 1);
    if(!mhash_table_insert_after_resize(table,key_element,value_element))
        return FALSE;
    else{
        table->table_size++;
        return TRUE;
    }
    
}


inline MHashTable mhash_table_make(size_t key_element_size,size_t value_element_size,
            mhash_function_t hasher,mhash_key_comparator comparator){

    return mhash_table_make_with_bucket(key_element_size,value_element_size,
                hasher,comparator,hash_table_prime_list[0]);
}
inline MHashTable mhash_table_make_int(size_t value_element_size){
    return mhash_table_make(sizeof(int),value_element_size,
            int_float_hash_function,int_float_comparator);
}
inline MHashTable mhash_table_make_float(size_t value_element_size){
    return mhash_table_make(sizeof(float),value_element_size,
                int_float_hash_function,int_float_comparator);
}
inline MHashTable mhash_table_make_long_long(size_t value_element_size){
    return mhash_table_make(sizeof(long long),value_element_size,
                ll_double_hash_function,ll_comparator);
}
inline MHashTable mhash_table_make_double(size_t value_element_size){
    return mhash_table_make(sizeof(double),value_element_size,
                ll_double_hash_function,ll_comparator);
}
inline MHashTable mhash_table_make_string(size_t value_element_size){
    return mhash_table_make(sizeof(MString),value_element_size,
                string_hash_function,string_comparator);
}

BOOL mhash_table_insert_key_string(MHashTable* hashtable,void* value_element,const char* key){
    MString key_string = make_string(key);
    BOOL rv = mhash_table_insert(hashtable,&key_string,value_element);
    destroy_string(&key_string);
    return rv;
}

BOOL mhash_table_remove(MHashTable* hashtable,void* key_element){
    return FALSE;
}

BOOL mhash_table_remove_string(MHashTable* hashtable,const char* key){
    MString key_string = make_string(key);
    BOOL rv = mhash_table_remove(hashtable,&key_string);
    destroy_string(&key_string);
    return rv;
}

BOOL mhash_table_visit(MHashTable* hashtable,void* key_element,void* dest_value_element){
    return FALSE;
}
BOOL mhash_table_visit_string(MHashTable* hashtable,const char* key,void* dest_value_element){
    MString key_string = make_string(key);
    BOOL rv = mhash_table_visit(hashtable,&key_string,dest_value_element);
    destroy_string(&key_string);
    return rv;

}

BOOL mhash_table_set(MHashTable* hashtable,void* key_element,void* value_element){
    return FALSE;
}

BOOL mhash_table_set_string(MHashTable* hashtable,const char* key,void* value_element){
    MString key_string = make_string(key);
    BOOL rv = mhash_table_visit(hashtable,&key_string,value_element);
    destroy_string(&key_string);
    return rv;
}


void mhash_table_destroy(MHashTable* hashtable){

    for(size_t i = 0;i != hashtable->table_bucket_size;i++){
        MLinkedList* list = (MLinkedList*)marray_data(&hashtable->hashtable,i);
        mlist_destroy(list);
    }

    hashtable->key_element = default_container_element_desc(0);
    hashtable->value_element = default_container_element_desc(0);
    hashtable->table_size = 0;
    hashtable->table_bucket_size = 0;
    hashtable->hasher = NULL;
    hashtable->comparator = NULL;

    marray_destroy(&hashtable->hashtable);
}