#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <term.h>

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

struct term_struct terminfo() {
  int columns = 80;
  int lines = 20;
  char *term_name = getenv("TERM");
  char *buff = NULL; /* tgetent(3) does not need buff on Linux */
  if (tgetent(buff, term_name)) {
    columns = tgetnum("co");
    lines = tgetnum("li");
  } else {
    fprintf(stderr, "failed to get terminal info");
  }
  struct term_struct term = {
    columns, lines
  };
  return term;
}
