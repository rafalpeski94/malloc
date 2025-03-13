#ifndef MALLOC_CUSTOM_H_
#define MALLOC_CUSTOM_H_


void *c_malloc(size_t argSize);
void c_free(void *argPtr);

//dostepna wolna pamiec w %
float c_free_mem_perc(void);

#endif /* MALLOC_CUSTOM_H_ */
