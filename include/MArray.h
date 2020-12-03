#ifndef _MArray_
#define _MArray_

#include "datadef.h"

typedef struct _MArray{
    void* data;
    ContainerElementDesc element;
    
    size_t end;
    size_t capacity;
} MArray;

MArray marray_make(size_t element_size);
MArray marray_make_with_size(size_t element_size,size_t size);

MArray marray_make_ed(ContainerElementDesc ed);
MArray marray_make_ed_with_size(size_t size,ContainerElementDesc ed);

void marray_destroy(MArray* marray);
void marray_clear(MArray* marray);

void marray_push_back(MArray* array,void* data);
void marray_push_back_multi(MArray* array,void* data,size_t num);
void marray_push_back_string(MArray* array,const char* str);

void marray_pop_back(MArray* array);
void marray_pop_back_multi(MArray* array,size_t num);

BOOL marray_visit(MArray* array,void* data,size_t index);
BOOL marray_visit_multi(MArray* array,void* data,size_t index,size_t num);

BOOL marray_set(MArray* array,void* data,size_t index);
BOOL marray_set_string(MArray* array,const char* data,size_t index);

size_t marray_size(MArray* array);

void* marray_data(MArray* marray,size_t index);
#endif