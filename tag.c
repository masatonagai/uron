#include "tag.h"

#include <string.h>

#include "uron.h"
#include "util.h"

int tagged(const struct uron_struct *uron, const char *tag) {
  int i, n;
  for (i = 0, n = (*uron).tag_n; i < n; i++) {
    if (strcmp((*uron).tags[i], tag) == 0) {
      return 1;
    }
  }
  return 0;
}

int tagstox(char **tagx, const char **tags, int tag_n) {
  (*tagx) = (char *) xmalloc(URON_TAG_MAX);
  size_t max_n = URON_TAG_MAX;
  int i;
  for (i = 0; i < tag_n; i++) {
    if (i > 0) {
      strncat(*tagx, URON_TAG_SP, max_n);
      max_n -= strlen(URON_TAG_SP);
    }
    strncat(*tagx, tags[i], max_n);
    max_n -= strlen(tags[i]);
  }
  return strlen(*tagx);
}

int gettags(char ***tags, const char *tagx) {
  (*tags) = NULL;
  char *pch;
  int tag_n = 0;
  char *_tagx = (char *) xmalloc(strlen(tagx) + 1);
  strncpy(_tagx, tagx, strlen(tagx));
  pch = strtok(_tagx, URON_TAG_SP);
  while (pch != NULL) {
    (*tags) = (char **) xrealloc((*tags), sizeof(char *) * (tag_n + 1));
    (*tags)[tag_n] = pch;
    tag_n++;
    pch = strtok(NULL, URON_TAG_SP);
  }
  return tag_n;
}

void addtag(const char *tag, const unsigned int *ids, int n) {
  /* TODO throw error if the tag contains space char */
  struct uron_struct **urons, *uron;
  int uron_c, i, j, b;
  uron_c = dgeturons(&urons);
  for (i = 0; i < uron_c; i++) {
    uron = urons[i];
    b = 0;
    for (j = 0; j < n; j++) {
      if ((*uron).id == ids[j]) {
        b = 1;
        break;
      }
    }
    if (!b) {
      continue;
    }
    (*uron).tag_n++;
    (*uron).tags = (char **) xrealloc((*uron).tags, sizeof(char *) * ((*uron).tag_n));
    (*uron).tags[(*uron).tag_n - 1] = strdup(tag);
    saveuron(uron);
    freeuron(uron);
  }
  free(urons);
}

void rmtag(const char *tag, const unsigned int *ids, int n) {
}
