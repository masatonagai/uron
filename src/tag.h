/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_TAG_H
#define URON_TAG_H

#define URON_TAG_MAX 32
#define URON_TAG_SP ","
#define URON_NO_TAGS "__no_tags"

#include "uron.h"
#include "types.h"

void addtag(cstring tag, cstring username, cstring tag_for_read,
    const unsigned int *ids, int n, cstring cron_dir);
void rmtag(cstring tag, cstring username, cstring tag_for_read, 
    const unsigned int *ids, int n, cstring cron_dir);
int tagstox(string *, cstring *, int);
int gettags(string **, cstring);
bool tagged(const Uron *, cstring);

#endif /* URON_TAG_H */
