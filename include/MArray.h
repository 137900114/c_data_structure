#ifndef _MArray_
#define _MArray_

#include "helper.h"

typedef struct _MArray{
    void* data;
    size_t element_size;
    
    size_t end;
    size_t capacity;
} MArray;

MArray marray_make(size_t element_size);
MArray marray_make_with_size(size_t element_size,size_t size);
void marray_destroy(MArray* marray);

void marray_push_back(MArray* array,void* data);
void marray_push_back_multi(MArray* array,void* data,size_t num);
void marray_pop_back(MArray* array);
void marray_pop_back_multi(MArray* array,size_t num);

BOOL marray_visit(MArray* array,void* data,size_t index);
BOOL marray_visit_multi(MArray* array,void* data,size_t index,size_t num);
size_t marray_size(MArray* array);
#endif