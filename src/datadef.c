#include "datadef.h"
#include <string.h>

void _default_constructor_def_t(void* dest){}
void _default_constructor_copy_t(void* dest,void* src,size_t ele_size){
    memcpy(dest,src,ele_size);
}
void _default_deconstructor_t(void* data){}
void _default_constructor_move_t(void* dest,void* src,size_t ele_size){
    memcpy(dest,src,ele_size);
}

ContainerElementDesc default_container_element_desc(size_t element_size){
    ContainerElementDesc desc;
    desc.element_size = element_size;
    desc.cons_copy = _default_constructor_copy_t;
    desc.cons_def = _default_constructor_def_t;
    desc.cons_move = _default_constructor_move_t;
    desc.decons = _default_deconstructor_t;

    return desc;
}