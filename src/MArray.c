#include "MArray.h"
#include "MMemoryBuffer.h"

#include <stdlib.h>
#include <string.h>

static const size_t default_array_capacity = 8;


static void* offset(void* data,size_t elem,size_t n){
    return (char*)data + elem * n;
}

MArray marray_make(size_t element_size){
    MArray array;

    array.capacity = default_array_capacity;
    array.element = default_container_element_desc(element_size);
    array.data = malloc(element_size * array.capacity);

    array.end = 0;
    return array;
}

MArray marray_make_with_size(size_t element_size,size_t size){
    MArray array;

    array.capacity = size * 2 > default_array_capacity ? size * 2 : default_array_capacity;
    array.element = default_container_element_desc(element_size);
    array.data = malloc(element_size * array.capacity);

    array.end = size;
    return array;
}

MArray marray_make_ed(ContainerElementDesc ed){
    MArray array;

    array.capacity = default_array_capacity;
    
    if(ed.cons_copy == NULL) ed.cons_copy = _default_constructor_copy_t;
    if(ed.cons_def == NULL) ed.cons_def = _default_constructor_def_t;
    if(ed.decons == NULL) ed.decons = _default_deconstructor_t;
    
    array.element = ed;
    array.data = malloc(ed.element_size * array.capacity);

    array.end = 0;
    return array;
}

MArray marray_make_ed_with_size(size_t size,ContainerElementDesc ed){
    MArray array;

    array.capacity = size * 2 > default_array_capacity ? size * 2 : default_array_capacity;

    if(ed.cons_copy == NULL) ed.cons_copy = _default_constructor_copy_t;
    if(ed.cons_def == NULL) ed.cons_def = _default_constructor_def_t;
    if(ed.decons == NULL) ed.decons = _default_deconstructor_t;
    
    array.element = ed;
    array.data = malloc(ed.element_size * array.capacity);

    array.end = size;
    for(size_t i = 0;i != size;i++){
        void* ele = offset(array.data,array.element.element_size,i);
        ed.cons_def(ele);
    }

    return array;
}

void marray_clear(MArray* array){
    for(size_t i = 0;i != array->end;i++){
        void* ele = offset(array->data,array->element.element_size,i);
        array->element.decons(ele);
    }
    array->end = 0;
}

void marray_destroy(MArray* array){
    marray_clear(array);

    free(array->data);
    array->capacity  =0;
    array->data = NULL;
}


inline void marray_push_back_string(MArray* array,const char* str){
    MString _str = make_string(str);
    marray_push_back(array,&_str);
    destroy_string(&_str);
}

inline void marray_push_back(MArray* array,void* data){
    marray_push_back_multi(array,data,1);
}

void marray_push_back_multi(MArray* array,void* data,size_t num){
    void* dest = NULL;
    //resize the whole marray
    if(array->capacity <= array->end + num){
        array->capacity = (array->capacity + num) * 2;

        void* new_data = malloc(array->capacity * array->element.element_size);
        for(size_t i = 0;i != array->end;i++){
            void* dest = offset(new_data,array->element.element_size,i);
            void* src = offset(array->data,array->element.element_size,i);
            array->element.cons_copy(dest,src,array->element.element_size);
        }
        free(array->data);
        array->data = new_data;

        dest = offset(array->data,array->element.element_size,array->end);
    }else{
        dest = offset(array->data,array->element.element_size,array->end);
    }

    for(size_t i = 0;i != num;i++){
        void* target = offset(dest,array->element.element_size,i);
        void* src = offset(data,array->element.element_size,i);
        array->element.cons_copy(target,src,array->element.element_size);
    }
    array->end += num;
}

void marray_pop_back_multi(MArray* array,size_t num){
    for(size_t i = 1;i <= num;i++){
        void* data = marray_data(array,array->end - i);
        array->element.decons(data);
    }
    array->end -= num;
}

void marray_pop_back(MArray* array){
    void* data = marray_data(array,array->end - 1);
    array->element.decons(data);

    array->end -= 1;
}

BOOL marray_visit(MArray* array,void* data,size_t index){
    if(index >= array->end) return FALSE;

    void* source = offset(array->data,array->element.element_size,index);
    array->element.cons_copy(data,source,array->element.element_size);

    return TRUE;
}

BOOL marray_visit_multi(MArray* array,void* data,size_t index,size_t num){
    if(index + num > array->end) return FALSE;

    void* source = offset(array->data,array->element.element_size,index);
    for(size_t i = 0;i != num;i++){
        void* src = offset(source,array->element.element_size,i);
        void* dest = offset(data,array->element.element_size,i);
        array->element.cons_copy(dest,src,array->element.element_size);
    }

    return TRUE;
}

BOOL marray_set(MArray* array,void* data,size_t index){
    void* dest = marray_data(array,index);
    if(dest == NULL) return FALSE;
    array->element.cons_copy(dest,data,array->element.element_size);
    return TRUE;
}

BOOL marray_set_string(MArray* array,const char* data,size_t index){
    MString str = make_string(data);
    BOOL rv = marray_set(array,&str,index);
    destroy_string(&str);
    return rv;
}

size_t marray_size(MArray* array){
    return array->end;
}

inline void* marray_data(MArray* array,size_t index){
    if(array->end <= index){
        return NULL;
    }
    return offset(array->data,array->element.element_size,index);
}

