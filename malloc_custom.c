#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> 
#include <string.h>
#include <assert.h>

#ifndef MALLOC_BUFF_SIZE
#define MALLOC_BUFF_SIZE 0xFFFF //max 0x1FFFFFF
#else
#if MALLOC_BUFF_SIZE > 0x1FFFFFF
#warning "MALLOC_BUFF_SIZE max 0x1FFFFFF"
#undef MALLOC_BUFF_SIZE
#define MALLOC_BUFF_SIZE 0x1FFFFFF
#endif
#endif

uint8_t buff[MALLOC_BUFF_SIZE];

#define IS_FREE_KEY 0x7A

typedef struct SBlock SBlock, *PSBlock;

__attribute__((packed)) struct SBlock 
{
  unsigned int size : 25;
  unsigned int is_free : 7;
  PSBlock next;
  PSBlock prev;
};  

#define BLOCK_SIZE sizeof(SBlock)



PSBlock start = NULL;
size_t freeMem = MALLOC_BUFF_SIZE;

#define align4(x) (((((x)-1) >> 2) << 2) + 4)

void *c_malloc(size_t argSize) {
    size_t size = align4(argSize);
    void *ret = NULL;
    if (NULL == start) { //init
        start = (PSBlock)buff;
        start->size = MALLOC_BUFF_SIZE - BLOCK_SIZE;
        start->is_free = IS_FREE_KEY;
        start->next = NULL;
        start->prev = NULL; 
        freeMem = MALLOC_BUFF_SIZE - BLOCK_SIZE;
    }

    // wyszukanie bloku
    PSBlock curr = start;
    while (curr) {
        if (IS_FREE_KEY == curr->is_free && curr->size >= size) {
            break;
        }
        curr = curr->next;
    }

    if (curr && size) {
        //jezeli po allokacji pozostala pamiec < BLOCK_SIZE wyrownaj do tej wartosci
        if(curr->size < size + BLOCK_SIZE){
            size = curr->size;
        }
        if (curr->size == size) {
            curr->is_free = 0;
            freeMem -= size;
        } else {
            // wydzielenie bloku
            PSBlock new_block = (PSBlock)((char *)(curr + 1) + size);
            new_block->size = curr->size - size - BLOCK_SIZE;
            new_block->is_free = IS_FREE_KEY;
            new_block->next = curr->next;
            new_block->prev = curr;
            if (curr->next) {
                curr->next->prev = new_block;
            }
            curr->size = size;
            curr->is_free = 0;
            curr->next = new_block;
            freeMem -= size + BLOCK_SIZE;
        }
        ret = (void *)(curr + 1);
    }
    return ret;
}

void c_free(void *argPtr){
    if(NULL == argPtr){
        return;
    }
    PSBlock curr = (PSBlock)((uint8_t *)argPtr - BLOCK_SIZE);
    if(IS_FREE_KEY == curr->is_free){ //zabezpieczenie przed podowjnym czyszczeniem
      return;
    }else if(argPtr < (void *)buff || argPtr > (void *)(buff + MALLOC_BUFF_SIZE)){
      //nie obslugiwany adres
      return;
    }else if(0 != curr->is_free){
      assert(false); //prawdopobnie pisanie poza buforem
    }
    curr->is_free = IS_FREE_KEY;
    freeMem += curr->size;
    // laczenie z nastepnym
    if (curr->next && IS_FREE_KEY == curr->next->is_free) {
        curr->size += BLOCK_SIZE + curr->next->size;
        curr->next = curr->next->next;
        if (curr->next) {
            curr->next->prev = curr;
        }
        freeMem += BLOCK_SIZE;
    }

    // laczenie z poprzednim
    if (curr->prev && IS_FREE_KEY == curr->prev->is_free) {
        curr->prev->size += BLOCK_SIZE + curr->size;
        curr->prev->next = curr->next;
        if (curr->next) {
            curr->next->prev = curr->prev;
        }
        freeMem += BLOCK_SIZE;
    }
}

void *c_calloc(size_t argSize){
    void *ptr = c_malloc(argSize);
    if (ptr) {
        memset(ptr, 0, argSize);
    }
    return ptr;
}

float c_free_mem_perc(void){
    return 100.0f * freeMem / MALLOC_BUFF_SIZE;
}
