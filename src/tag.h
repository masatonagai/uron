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

void addtag(const string tag, const string username, const string tag_for_read,
    const unsigned int *ids, int n, const string cron_dir);
void rmtag(const string tag, const string username, const string tag_for_read, 
    const unsigned int *ids, int n, const string cron_dir);
int tagstox(string *, const string *, int);
int gettags(string **, const string);
int tagged(const struct uron_struct *, const string);

#endif /* URON_TAG_H */
