#include "MMemoryPool.h"

static BOOL is_memory_pool_initialized = FALSE;

static const size_t block_size_table[] = {
    //round up 8
	8,16,24,32,40,48,56,64,72,80,88,96,
	104,112,120,128,
	//round up 32
	160,192,224,256,288,320,352,384,416,
	448,480,512,
	//round up 64
	576,640,704,768,832,896,960,1024
};

//base must be the power of 2
static size_t round_up(size_t target, size_t base) {
	return (target + base - 1) & ~(base - 1);
}


static size_t get_block_index(size_t size) {
	if (size <= 128) return round_up(size, 8) / 8 - 1;
	else if (size <= 512) return round_up(size - 128, 32) / 32 + 15;
	else return round_up(size - 512, 64) / 64 + 27;
}

static size_t get_block_size(size_t index) { return block_size_table[index]; }

static const size_t page_size = 1 << 16;

static void* memory_page_table[_countof(block_size_table)];

typedef struct _memory_page_table_block{
    void* data;
    struct _memory_page_table_block* next;
} _memory_page_table_block;

static _memory_page_table_block* memory_page_blocks[_countof(block_size_table)];

#include "MArray.h"

static MArray used_memory_pages;

#include <stdlib.h>

static _memory_page_table_block* inittialize_page(void* page,size_t block_size){
    _memory_page_table_block* head = (_memory_page_table_block*)page;
    head->data = head;
    head->next = NULL;
    _memory_page_table_block* current = head;
    size_t block_num = page_size / block_size;
    for(int j = 1;j < block_num;j++){
        _memory_page_table_block* next = (_memory_page_table_block*)(((char*)current) + block_size);
        next->data = next;
        next->next = current->next;
        current->next = next;

        current = current->next;
    }

    return head;
}

BOOL mempool_initialize(){
    if(is_memory_pool_initialized) return TRUE;

    for(int i = 0;i < _countof(block_size_table);i++){
        memory_page_table[i] = malloc(page_size);
        if(memory_page_table[i] == NULL){
            mempool_finalize();
            return FALSE;
        }

        size_t block_size = get_block_size(i);
        memory_page_blocks[i] = inittialize_page(memory_page_table[i],block_size);
    }
    used_memory_pages = marray_make(sizeof(void*));

    is_memory_pool_initialized = TRUE;
    return TRUE;
}

void mempool_finalize(){

    size_t i;
    for(i = 0;i != _countof(memory_page_table);i++){
        free(memory_page_table[i]);
    }
    
    size_t used_page_num = marray_size(&used_memory_pages);
    for(i = 0;i != used_page_num;i++){
        void* page;
        marray_visit(&used_memory_pages,&page,i);

        if(page != NULL) free(page);
    }

    marray_destroy(&used_memory_pages);
    is_memory_pool_initialized = FALSE;
}

void* mempool_allocate(size_t size){
    if(!is_memory_pool_initialized)
        mempool_initialize();

    if(size > block_size_table[_countof(block_size_table) - 1]){
        //no block in the block table is suitable for the memory size
        //use memory allocated by system
        return malloc(size); 
    }

    size_t block_index = get_block_index(size);
    _memory_page_table_block* current_block = memory_page_blocks[block_index];
    //every block on this page is used up,create a new page
    if(current_block == NULL){
        marray_push_back(&used_memory_pages,&memory_page_table[block_index]);
        void* newpage = malloc(page_size);
        if(newpage == NULL){
            //out of memory,return NULL
            return NULL;
        }
        memory_page_table[block_index] = newpage;
        memory_page_blocks[block_index] = inittialize_page(newpage,get_block_size(block_index));
        current_block = memory_page_blocks[block_index];
    }

    memory_page_blocks[block_index] = current_block->next;
    return (void*)current_block;
}

void mempool_deallocate(void* mem,size_t size){
    if(!is_memory_pool_initialized)
        return;
    
    if(size > block_size_table[_countof(block_size_table) - 1]){
        return free(mem);
    }

    size_t block_index = get_block_index(size);
    _memory_page_table_block* new_block = (_memory_page_table_block*)(mem);

    new_block->next = memory_page_blocks[block_index];
    memory_page_blocks[block_index] = new_block;
}
