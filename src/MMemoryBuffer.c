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
    MString mstr;
    if(str == NULL){
        mstr.data = NULL;
        mstr.size = 0;
    }else{
        int size = strlen(str) + 1;
        mstr = make_membuffer(size);
        if(mstr.data != NULL){
            memcpy(mstr.data,str,size);
            *((char*)mstr.data + mstr.size) = '\0';
        }
    }
    return mstr;
}

inline int compare_string(MString* mstr,const char* str){
    return strcmp(mstr->data,str);
}

inline int compare_string_string(MString* lhs,MString* rhs){
    return compare_string(lhs,(char*)rhs->data);
}

inline char visit_string(MString* mstr,size_t index){
    if(index < mstr->size - 1){
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

void cat_string(MString* dest,const char* str){
    if(dest->data == NULL){
        *dest = make_string(str);
        return;
    }
    if(str == NULL){
        return;
    }
    size_t str_size = strlen(str);
    MString newstr = make_membuffer(dest->size + str_size);
    strcpy((char*)newstr.data,(char*)dest->data);
    strcat((char*)newstr.data,str);

    destroy_string(dest);
    *dest = newstr;
}

inline void cat_string_string(MString* dest,MString* src){
    cat_string(dest,(char*)src->data);
}

inline size_t string_len(MString* str){
    if(str->data) return 0;
    return str->size - 1;
}

inline char* string_data(MString* str){
    return (char*)str->data;
}

static void string_constructor_def(void* src){
    MString* str = (MString*)src;
    if(str->data != NULL)
        destroy_string(str);
    str->data = NULL;
    str->size = 0;
}

static void string_constructor_copy(void* dest,void* src,size_t trash){
    MString* src_str = (MString*)src;
    MString* dest_str = (MString*)dest;
    if(dest_str->data != NULL){
        destroy_string(dest_str);
    }
    *dest_str = make_string((char*)src_str->data);
}

static void string_deconstructor(void* data){
    destroy_string((MString*)data);
}

inline ContainerElementDesc string_element_descriptor(){
    ContainerElementDesc desc;
    desc.element_size = sizeof(MString);
    desc.cons_copy = string_constructor_copy;
    desc.cons_def = string_constructor_def;
    desc.decons = string_deconstructor;
    return desc;
}