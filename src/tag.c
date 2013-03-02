/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "tag.h"
#include "uron.h"
#include "types.h"
#include "util.h"

#include <string.h>

bool tagged(const uron_t *uron, const string_t tag) {
  if (strcmp(tag, URON_NO_TAGS) == 0) {
    return (*uron).tag_n == 0;
  }
  for (int i = 0, n = (*uron).tag_n; i < n; i++) {
    if (strcmp((*uron).tags[i], tag) == 0) {
      return true;
    }
  }
  return false;
}

int tagstox(string_t *tagx, const string_t *tags, int tag_n) {
  (*tagx) = (string_t) xmalloc(URON_TAG_MAX);
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

int gettags(string_t **tags, const string_t tagx) {
  (*tags) = NULL;
  string_t pch;
  int tag_n = 0;
  string_t _tagx = (string_t) xmalloc(strlen(tagx) + 1);
  strncpy(_tagx, tagx, strlen(tagx));
  pch = strtok(_tagx, URON_TAG_SP);
  while (pch != NULL) {
    (*tags) = (string_t *) xrealloc((*tags), sizeof(string_t) * (tag_n + 1));
    (*tags)[tag_n] = pch;
    tag_n++;
    pch = strtok(NULL, URON_TAG_SP);
  }
  return tag_n;
}

void addtag(const string_t tag_for_write, const string_t username, const string_t tag_for_read,
    const unsigned int *ids, int n, const string_t cron_dir) {
  if (tag_for_read && strcmp(tag_for_write, tag_for_read) == 0) {
    // already tagged
    return;
  }
  // TODO throw error if the tag contains space string_t
  uron_t **urons;
  int uron_c = geturons(&urons, username, tag_for_read, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    uron_t *uron = urons[i];
    (*uron).tag_n++;
    (*uron).tags = (string_t *) xrealloc((*uron).tags, sizeof(string_t) * ((*uron).tag_n));
    (*uron).tags[(*uron).tag_n - 1] = strdup(tag_for_write);
    saveuron(uron);
    freeuron(uron);
  }
  free(urons);
}

void rmtag(const string_t tag_for_write, const string_t username, const string_t tag_for_read,
    const unsigned int *ids, int n, const string_t cron_dir) {
  uron_t **urons;
  int uron_c = geturons(&urons, username, tag_for_read, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    uron_t *uron = urons[i];
    for (int k = 0; k < (*uron).tag_n; k++) {
      if (strcmp((*uron).tags[k], tag_for_write) == 0) {
        string_t removed_tag = (*uron).tags[k];
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
