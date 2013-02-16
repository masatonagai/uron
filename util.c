#include "util.h"
#include <stdlib.h>
#include <stdio.h>

void * xmalloc(size_t size) {
  void *p;
  /* calloc(3) sets the memory to zero */
  p = calloc(1, size);
  if (!p) {
    fprintf(stderr,
        "failed to allocate memory: size=%lu\n",
        (unsigned long) size);
    exit(EXIT_FAILURE);
  }
  return p;
}

void * xrealloc(void *p, size_t size) {
  void *np;
  np = realloc(p, size);
  if (!np) {
    fprintf(stderr,
        "failed to reallocate memory: size=%lu\n",
        (unsigned long) size);
    exit(EXIT_FAILURE);
  }
  return np;
}
