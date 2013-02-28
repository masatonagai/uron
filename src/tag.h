#ifndef URON_TAG_H
#define URON_TAG_H

#define URON_TAG_MAX 32
#define URON_TAG_SP ","
#define URON_NO_TAGS "__no_tags"

#include "uron.h"

void addtag(const char *tag, const char *tag_for_read, const unsigned int *ids, int n, 
    const char *cron_dir);
void rmtag(const char *tag, const char *tag_for_read, const unsigned int *ids, int n,
    const char *cron_dir);
int tagstox(char **, const char **, int);
int gettags(char ***, const char *);
int tagged(const struct uron_struct *, const char *);

#endif /* URON_TAG_H */
