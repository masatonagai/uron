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
int regmatch(string_t **match, const string_t s, const string_t pattern, int max);
void regmatchfree(string_t **match, int match_c);

#endif /* URON_UTIL_H */
