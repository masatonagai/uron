#include "term.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 ||
      ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) != -1 ||
      ioctl(STDIN_FILENO,  TIOCGWINSZ, &ws) != -1) {
      cols = ws.ws_col;
      rows = ws.ws_row;
    }
  }
  if (!(cols && rows)) {
    int fd = open("/dev/tty", O_NOCTTY|O_NONBLOCK|O_RDONLY);
    if(fd != -1){
      struct winsize ws;
      int ret = ioctl(fd, TIOCGWINSZ, &ws);
      close(fd);
      if(ret != -1) {
        cols = ws.ws_col;
        rows = ws.ws_row;
      }
    }
  }
  if (!(cols && rows)) {
    fprintf(stderr, "failed to get terminal info\n");
    cols = 80;
    rows = 24;
  }
  struct term_struct t = {
    cols, rows
  };
  return t;
}
