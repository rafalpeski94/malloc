#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "malloc_custom.h"



int main(int i, char** x){
  //testy
  void *a,*b,*c,*d,*e;
  a = c_malloc(10);
  memset(a, 0xA, 10);
  b = c_malloc(10);
  memset(b, 0xB, 10);
  c = c_malloc(10);
  memset(c, 0xC, 10);
  d = c_malloc(10);
  memset(d, 0xD, 10);
  e = c_malloc(10);
  memset(e, 0xE, 10);
  printf("%x %x", *(uint8_t *)a, *(uint8_t *)(a + 5)); //oczekiwane a a
  c_free(a);
  c_free(b);
  c_free(d);
  c_free(e);
  c_free(b);
  c_free(c);
  a = c_malloc(10);
  b = c_malloc(10);
  c_free(a);
  c = c_malloc(8);
  d = c_malloc(10);
  c_free(d);
  e = c_malloc(10);
  a = c_malloc(10);
  d = c_malloc(10);
  c_free(b);
  c_free(c);
  c_free(d);
  c_free(a);
  c_free(b);
  c_free(e);
  a = c_malloc(0);
  c_free(a);
}
