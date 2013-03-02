/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_UTIL_H
#define URON_UTIL_H

#include "types.h"

#include <stdlib.h>

void * xmalloc(size_t size);
void * xrealloc(void *p, size_t size);
int regmatch(string **match, const string s, const string pattern, int max);
void regmatchfree(string **match, int match_c);

#endif /* URON_UTIL_H */
