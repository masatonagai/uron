/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_UTIL_H
#define URON_UTIL_H

#include <stdlib.h>

void * xmalloc(size_t size);
void * xrealloc(void *p, size_t size);
int regmatch(char ***match, const char *s, const char *pattern, int max);
void regmatchfree(char ***match, int match_c);

#endif /* URON_UTIL_H */
