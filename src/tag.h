#ifndef URON_TAG_H
#define URON_TAG_H

#define URON_TAG_MAX 32
#define URON_TAG_SP ","

#include "uron.h"

void addtag(const char *, const unsigned int *, int);
void rmtag(const char *, const unsigned int *, int);
int tagstox(char **, const char **, int);
int gettags(char ***, const char *);
int tagged(const struct uron_struct *, const char *);

#endif /* URON_TAG_H */
