#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

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

int regmatch(char ***match, const char *s, const char *pattern, int max) {
  (*match) = (char **) xmalloc(sizeof(char *) * max);

  regex_t regex;
  regmatch_t pmatch[max];
  //regmatch_t *pmatch = (regmatch_t *) xmalloc(sizeof(regmatch_t) * max);
  if (regcomp(&regex, pattern, REG_EXTENDED) != REG_NOERROR) {
    fprintf(stderr, "faild to compile regex. pattern=%s\n", pattern);
    exit(EXIT_FAILURE);
  }
  if (regexec(&regex, s, (size_t) max, pmatch, 0) != REG_NOERROR) {
    return 0;
    regfree(&regex);
  }
  regfree(&regex);

  int match_c;
  for (match_c = 0; match_c < max; match_c++) {
    regmatch_t m = pmatch[match_c];
    if (m.rm_so == -1) {
      break;
    }
    size_t len = m.rm_eo - m.rm_so;
    (*match)[match_c] = (char *) xmalloc(len + 1);
    strncpy((*match)[match_c], s + m.rm_so, len);
    (*match)[match_c][len] = '\0';
  }
  // free(pmatch);

  return match_c;
}