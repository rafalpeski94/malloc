#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> 
#include <string.h>

#ifndef MALLOC_BUFF_SIZE
#define MALLOC_BUFF_SIZE 0xFFFF //max 0x7FFFFFFF
#endif

uint8_t buff[MALLOC_BUFF_SIZE];

typedef struct SBlock SBlock, *PSBlock;

__attribute__((packed)) struct SBlock 
{
  unsigned int size : 31;
  unsigned int is_free : 1;
  PSBlock next;
  PSBlock prev;
}; 

#define BLOCK_SIZE sizeof(SBlock)
PSBlock start = NULL;
size_t freeMem = MALLOC_BUFF_SIZE;
void *c_malloc(size_t argSize) {
    size_t size = argSize;
    void *ret = NULL;
    if (NULL == start) { //init
        start = (PSBlock)buff;
        start->size = MALLOC_BUFF_SIZE - BLOCK_SIZE;
        start->is_free = 1;
        start->next = NULL;
        start->prev = NULL; 
        freeMem = MALLOC_BUFF_SIZE - BLOCK_SIZE;
    }

    // wyszukanie bloku
    PSBlock curr = start;
    while (curr) {
        if (curr->is_free && curr->size >= size) {
            break;
        }
        curr = curr->next;
    }

    if (curr) {
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
            new_block->is_free = 1;
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
    PSBlock curr = (PSBlock)((uint8_t *)argPtr - BLOCK_SIZE);
    if(curr->is_free){ //zabezpieczenie przed podowjnym czyszczeniem
      return;
    }
    curr->is_free = 1;
    freeMem += curr->size;
#ifdef MEMFREE_SET_ZERO
    memset((void *)(curr + 1), 0x00, curr->size);
#endif
    // laczenie z nastepnym
    if (curr->next && curr->next->is_free) {
        curr->size += BLOCK_SIZE + curr->next->size;
        curr->next = curr->next->next;
        if (curr->next) {
            curr->next->prev = curr;
        }
        freeMem += BLOCK_SIZE;
#ifdef MEMFREE_SET_ZERO
        memset((void *)(curr + 1), 0x00, curr->size);
#endif
    }

    // laczenie z poprzednim
    if (curr->prev && curr->prev->is_free) {
        curr->prev->size += BLOCK_SIZE + curr->size;
        curr->prev->next = curr->next;
        if (curr->next) {
            curr->next->prev = curr->prev;
        }
        freeMem += BLOCK_SIZE;
#ifdef MEMFREE_SET_ZERO
        memset((void *)(curr->prev + 1), 0x00, curr->prev->size);
#endif
    }
}

float c_free_mem_perc(void){
    return 100.0f * freeMem / MALLOC_BUFF_SIZE;
}
