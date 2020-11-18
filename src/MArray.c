#include "MArray.h"

#include <stdlib.h>

#include <string.h>

static const size_t default_array_capacity = 8;

MArray marray_make(size_t element_size){
    MArray array;

    array.capacity = default_array_capacity;
    array.element_size = element_size;
    array.data = malloc(element_size * array.capacity);

    array.end = 0;
    return array;
}

MArray marray_make_with_size(size_t element_size,size_t size){
    MArray array;

    array.capacity = size * 2 > default_array_capacity ? size * 2 : default_array_capacity;
    array.element_size = element_size;
    array.data = malloc(element_size * array.capacity);

    array.end = 0;
    return array;
}

void marray_distroy(MArray* array){
    free(array->data);
    array->capacity  =0;
    array->element_size = 0;
    array->data = NULL;
    array->end = 0;
}

static void* offset(void* data,size_t elem,size_t n){
    return (char*)data + elem * n;
}

void marray_push_back(MArray* array,void* data){
    marray_push_back_multi(array,data,1);
}

void marray_push_back_multi(MArray* array,void* data,size_t num){
    void* dest = NULL;
    //resize the whole marray
    if(array->capacity <= array->end + num){
        array->capacity = (array->capacity + num) * 2;

        void* new_data = malloc(array->capacity * array->element_size);
        memcpy(new_data,array->data,array->end * array->element_size);
        free(array->data);
        array->data = new_data;

        dest = offset(array->data,array->element_size,array->end);
    }else{
        dest = offset(array->data,array->element_size,array->end);
    }

    memcpy(dest,data,array->element_size);
    array->end += num;
}

void marray_pop_back_multi(MArray* array,size_t num){
    array->end -= num;
}

void marray_pop_back(MArray* array){
    array->end -= 1;
}

BOOL marray_visit(MArray* array,void* data,size_t index){
    if(index >= array->end) return FALSE;

    void* source = offset(array->data,array->element_size,index);
    memcpy(data,source,array->element_size);

    return TRUE;
}

BOOL marray_visit_multi(MArray* array,void* data,size_t index,size_t num){
    if(index + num > array->end) return FALSE;

    void* source = offset(array->data,array->element_size,index);
    memcpy(data,source,array->element_size * num);

    return TRUE;
}

size_t marray_size(MArray* array){
    return array->end;
}