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
    _memory_page_table_block* next;
} _memory_page_table_block;

static _memory_page_table_block* memory_page_blocks[_countof(block_size_table)];

#include <stdlib.h>

BOOL mempool_initialize(){
    if(is_memory_pool_initialized) return;

    for(int i = 0;i < _countof(block_size_table);i++){
        memory_page_table[i] = malloc(page_size);
        if(memory_page_table[i] == NULL){
            mempool_finalize();
            return FALSE;
        }

        size_t block_size = get_block_size(i);
        _memory_page_table_block* head = (_memory_page_table_block*)memory_page_table[i];
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

        memory_page_blocks[i] = head;
    }





}