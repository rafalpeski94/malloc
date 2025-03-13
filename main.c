#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "malloc_custom.h"
#include <winbase.h>



int main(int i, char** x){
  //testy
  void *a,*b,*c,*d,*e;
  a = c_malloc(10);
  b = c_malloc(10);
  c = c_malloc(10);
  d = c_malloc(10);
  e = c_malloc(10);
  c_free(a);
  c_free(b);
  c_free(d);
  c_free(e);
  c_free(b);
  c_free(c);
  a = c_malloc(10);
  b = c_malloc(10);
  c_free(a);
  c = c_malloc(10);
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
}
