#include "MMemoryBuffer.h"
#include "MMemoryPool.h"
#include <string.h>

MemoryBuffer make_membuffer(size_t size){
    MemoryBuffer buffer;
    
    buffer.data = mempool_allocate(size);
    buffer.size = size;
    return buffer;
}

void destroy_membuffer(MemoryBuffer* buffer){
    if(buffer->data != NULL){
        mempool_deallocate(buffer->data,buffer->size);
        buffer->data = NULL;
        buffer->size = 0;
    }
}

inline BOOL check_membuffer_validity(MemoryBuffer* buffer){
    return buffer->data != NULL;
}

inline void* membuffer_offset(MemoryBuffer* buffer,size_t offset){
    if(offset >= buffer->size) return NULL;
    return (char*)buffer->data + offset;
}

inline size_t membuffer_size(MemoryBuffer* buffer){
    return buffer->size;
}

inline void membuffer_zero(MemoryBuffer* buffer){
    memset(buffer->data,0,buffer->size);
}

MString make_string(const char* str){
    int size = strlen(str);
    MString mstr = make_membuffer(size);
    if(mstr.data != NULL){
        memcpy(mstr.data,str,size);
        mstr.size = size;
    }
    return mstr;
}

inline int compare_string(MString* mstr,const char* str){
    return memcmp(mstr->data,str,mstr->size);
}

inline char visit_string(MString* mstr,size_t index){
    if(index < mstr->size){
        char* data = (char*)mstr->data;
        return data[index];        
    }
    return '\0';
}

inline void destroy_string(MString* mstr){
    destroy_membuffer(mstr);
}

void copy_string(MString* dest,MString* src){
    destroy_string(dest);
    *dest = make_membuffer(src->size);
    memcpy(dest->data,src->data,src->size);
}

void cat_string(MString* dest,MString* src){
    if(dest->data == NULL){
        copy_string(dest,src);
        return;
    }
    if(src->data == NULL){
        return;
    }
    MString newstr = make_membuffer(dest->size + src->size);
    memcpy(newstr.data,dest->data,dest->size);
    char* srcdata = (char*)newstr.data + dest->size;
    memcpy(srcdata,src->data,src->size);

    destroy_string(dest);
    *dest = newstr;
}