#ifndef _MHashTable_
#define _MHashTable_

#include "datadef.h"
#include "MArray.h"


typedef size_t (* mhash_function_t)(void* data,size_t table_size);
typedef BOOL (* mhash_key_comparator)(void* dest,void* src);


typedef struct _MHashTable{
    MArray hashtable;

    size_t key_element_size;
    size_t value_element_size;

    mhash_function_t hasher;
    mhash_key_comparator comparator;

    size_t table_bucket_size;
    size_t table_size;
} MHashTable;

MHashTable mhash_table_make(size_t key_element_size,size_t value_element_size,
            mhash_function_t hasher,mhash_key_comparator comparator);
MHashTable mhash_table_make_int(size_t value_element_size);
MHashTable mhash_table_make_float(size_t value_element_size);
MHashTable mhash_table_make_long_long(size_t value_element_size);
MHashTable mhash_table_make_double(size_t value_element_size);
MHashTable mhash_table_make_string(size_t value_element_size);

BOOL mhash_table_insert(MHashTable* hashtable,void* value_element,void* key_element);
BOOL mhash_table_insert_key_string(MHashTable* hashtable,void* value_element,const char* key);

BOOL mhash_table_remove(MHashTable* hashtable,void* key_element);
BOOL mhash_table_remove_string(MHashTable* hashtable,const char* key);

BOOL mhash_table_visit(MHashTable* hashtable,void* key_element,void* dest_value_element);
BOOL mhash_table_visit_string(MHashTable* hashtable,const char* key,void* dest_value_element);

BOOL mhash_table_set(MHashTable* hashtable,void* key_element,void* value_element);
BOOL mhash_table_set_string(MHashTable* hashtable,const char* key_element,void* value_element);

void mhash_table_destroy(MHashTable* hashtable);
#endif