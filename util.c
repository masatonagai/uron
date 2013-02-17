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
  /* don't use "columns" or "lines" that are macro defined in term.h */
  int cols = 0;
  int rows = 0;
  char *s;
  s = getenv("COLUMNS");
  if (s && *s) {
    cols = atoi(s);
  }
  s = getenv("LINES");
  if (s && *s) {
    rows = atoi(s);
  }
  if (!(cols && rows)) {
    char *term_name = getenv("TERM");
    if (term_name && *term_name) {
      char buff[2048];
      if (tgetent(buff, term_name)) {
        if (!cols) {
          cols = tgetnum("co");
        }
        if (!rows) {
          rows = tgetnum("li");
        }
      }
    }
  }
  if (!(cols && rows)) {
    fprintf(stderr, "failed to get terminal info");
    cols = 80;
    rows = 25;
  }
  struct term_struct t = {
    cols, rows
  };
  printf ("screen: %d * %d\n", cols, rows);
  return t;
}
