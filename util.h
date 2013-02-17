#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

void * xmalloc(size_t size);
void * xrealloc(void *p, size_t size);

struct term_struct {
  int columns;
  int lines;
};
struct term_struct terminfo();

#endif /* UTIL_H */
