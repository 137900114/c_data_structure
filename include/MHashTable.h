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

    size_t table_size;
} MHashTable;

MHashTable mhash_table_make(size_t key_element_size,size_t value_element_size,
            mhash_function_t hasher,mhash_key_comparator comparator);
MHashTable mhash_table_make_int(size_t value_element_size);
MHashTable mhash_table_make_float(size_t value_element_size);
MHashTable mhash_table_make_long_long(size_t value_element_size);
MHashTable mhash_table_make_double(size_t value_element_size);
MHashTable mhash_table_make_string(size_t value_element_size);

BOOL mhash_table_insert(MHashTable* hashtable,const void* value_element);
BOOL mhash_table_visit(MHashTable* hashtable,const void* key_element,void* dest_value_element);

void mhash_table_destroy(MHashTable* hashtable);
#endif