#ifndef _MMemoryPool_
#define _MMemoryPool_

#include "helper.h"

BOOL mempool_initialize();

void* mempool_allocate(size_t size);
void mempool_deallocate(void* data,size_t size);

void mempool_finalize();

#endif