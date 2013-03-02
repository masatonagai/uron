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

void addtag(const string_t tag, const string_t username, const string_t tag_for_read,
    const uint32_t *ids, int n, const string_t cron_dir);
void rmtag(const string_t tag, const string_t username, const string_t tag_for_read, 
    const uint32_t *ids, int n, const string_t cron_dir);
int tagstox(string_t *, const string_t *, int);
int gettags(string_t **, const string_t);
bool tagged(const uron_t *, const string_t);

#endif /* URON_TAG_H */
