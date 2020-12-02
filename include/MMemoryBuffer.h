#ifndef _membuffer_
#define _membuffer_

#include <stddef.h>
#include "datadef.h"

typedef struct _MemoryBuffer{
    void* data;
    size_t size;
} MemoryBuffer;


MemoryBuffer make_membuffer(size_t size);
void destroy_membuffer(MemoryBuffer* buffer);

void* membuffer_offset(MemoryBuffer* buffer,size_t offset);
size_t membuffer_size(MemoryBuffer* buffer);
void membuffer_zero(MemoryBuffer* buffer);

typedef MemoryBuffer MString;
MString make_string(const char* str);
void copy_string(MString* dest,MString* src);

int compare_string(MString* mstr,const char* str);
char visit_string(MString* mstr,size_t index);
void destroy_string(MString* mstr);

void cat_string(MString* dest,MString* other);
BOOL string_data(char* buffer,size_t buffer_size,MString* str);

#endif