#ifndef _datadef_
#define _datadef_

#ifndef BOOL

#define BOOL int
#define TRUE 1
#define FALSE 0

#endif

typedef float real;

#ifndef _countof
#define _countof(array) sizeof(array) / sizeof(*array)
#endif

#include <stdio.h>
#include <stddef.h>

typedef void (*constructor_def_t)(void* dest);
typedef void (*constructor_copy_t)(void* dest,void* src,size_t ele_size);
typedef void (*constructor_move_t)(void* dest,void* src,size_t ele_size);
typedef void (*deconstructor_t)(void* data);

typedef struct _ContainerElementDesc{
    constructor_def_t cons_def;
    constructor_copy_t cons_copy;
    constructor_move_t cons_move;
    deconstructor_t decons;

    size_t element_size;
} ContainerElementDesc;

void _default_constructor_move_t(void* dest,void* src,size_t size);
void _default_constructor_def_t(void* dest);
void _default_constructor_copy_t(void* dest,void* src,size_t size);
void _default_deconstructor_t(void* data);

ContainerElementDesc default_container_element_desc(size_t element_size);
#endif