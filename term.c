#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <term.h>

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
  return t;
}
