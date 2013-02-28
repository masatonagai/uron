#include "term.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#if HAVE_LIBCURSES
#include <curses.h>
#include <term.h>
#endif /* HAVE_LIBCURSES */

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
#if HAVE_LIBCURSES
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
#endif /* HAVE_LIBCURSES */
  if (!(cols && rows)) {
    struct winsize ws;
    ioctl(1, TIOCGWINSZ, &ws);
    cols = ws.ws_col;
    rows = ws.ws_row;
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
