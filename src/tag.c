#include "tag.h"

#include <string.h>

#include "uron.h"
#include "util.h"

int tagged(const struct uron_struct *uron, const char *tag) {
  if (strcmp(tag, URON_NO_TAGS) == 0) {
    return (*uron).tag_n == 0 ? 1 : 0;
  }
  for (int i = 0, n = (*uron).tag_n; i < n; i++) {
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

void addtag(const char *tag_for_write, const char *tag_for_read, const unsigned int *ids, int n,
    const char *cron_dir) {
  if (tag_for_read && strcmp(tag_for_write, tag_for_read) == 0) {
    // already tagged
    return;
  }
  /* TODO throw error if the tag contains space char */
  struct uron_struct **urons;
  int uron_c = geturons(&urons, tag_for_read, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    (*uron).tag_n++;
    (*uron).tags = (char **) xrealloc((*uron).tags, sizeof(char *) * ((*uron).tag_n));
    (*uron).tags[(*uron).tag_n - 1] = strdup(tag_for_write);
    saveuron(uron);
    freeuron(uron);
  }
  free(urons);
}

void rmtag(const char *tag_for_write, const char *tag_for_read, const unsigned int *ids, int n,
    const char *cron_dir) {
  struct uron_struct **urons;
  int uron_c = geturons(&urons, tag_for_read, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    for (int k = 0; k < (*uron).tag_n; k++) {
      if (strcmp((*uron).tags[k], tag_for_write) == 0) {
        char *removed_tag = (*uron).tags[k];
        for (int l = k + 1; l < (*uron).tag_n; l++) {
          (*uron).tags[l - 1] = strdup((*uron).tags[l]);
        }
        (*uron).tag_n--;
        saveuron(uron);
        free(removed_tag);
        break;
      }
    }
    freeuron(uron);
  }
  free(urons);
}
