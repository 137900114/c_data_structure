#include "MHashTable.h"
#include "MMemoryBuffer.h"
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
    
}