/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "uron.h"
#include "types.h"
#include "cron.h"
#include "path.h"
#include "util.h"
#include "term.h"
#include "io.h"
#include "tag.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>

#define URON_LINE_MAX 1024 /* includes newline character */
#define URON_LINE_PADCHAR '\r'
#define URON_ID_MIN 1

void freeuron(uron_t *uron) {
  freecron((*uron).cron);
  free(uron);
}

static uron_t * getlasturon(int fd) {
  off_t eof = lseek(fd, 0, SEEK_END);
  if (eof > 0) {
    off_t o = lseek(fd, (off_t) (eof - URON_LINE_MAX), SEEK_SET);
    if (o >= 0) {
      string_t luronx = (string_t) xmalloc(URON_LINE_MAX);
      read(fd, luronx, URON_LINE_MAX);
      uron_t *luron = geturon(luronx);
      free(luronx);
      return luron;
    }
  }
  return NULL;
}

void saveuron(uron_t *uron) {
  /*
  printf("open %s\n", URON_DIR);
  DIR *uron_dir = opendir(URON_DIR);
  if (uron_dir == NULL) {
    fprintf(stderr, "failed to open \"%s\"\n", URON_DIR);
    exit(EXIT_FAILURE);
  }
  */
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", URON_DIR, "1");

  int fd = open(path, O_RDWR | O_CREAT | O_SYNC);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // lock the uron file for preserving uniqueness of id
  if (flock(fd, LOCK_EX) == -1) {
    perror("flock");
    exit(EXIT_FAILURE);
  }

  int add = !(*uron).id;
  if (add) {
    (*uron).id = URON_ID_MIN;
    uron_t *luron = getlasturon(fd);
    if (luron != NULL) {
      (*uron).id = (*luron).id + 1;
      free(luron);
    }
  } else {
    lseek(fd, (off_t) (URON_LINE_MAX * ((*uron).id - URON_ID_MIN)), SEEK_SET);
  }

  string_t cronx, tagx;
  crontox(&cronx, (*uron).cron);
  tagstox(&tagx, (const string_t *) (*uron).tags, (*uron).tag_n);

  char line[URON_LINE_MAX + 1];
  int len = snprintf(line, sizeof(line), "%d %s %s", (*uron).id, tagx, cronx);

  free(cronx);
  free(tagx);

  // right pad
  int i, n;
  for (i = len, n = URON_LINE_MAX - 1; i < n; i++) {
    line[i] = URON_LINE_PADCHAR;
  }
  line[URON_LINE_MAX - 1] = '\n';
  line[URON_LINE_MAX] = '\0';

  ssize_t wb = write(fd, line, strlen(line));

  // don't forget unlock the uron file
  flock(fd, LOCK_UN);

  if (wb == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
}

uron_t * makeuron(cron_t *cron) {
  uron_t *uron = (uron_t *) xmalloc(sizeof(uron_t));
  (*uron).id = 0;
  (*uron).tag_n = 0;
  (*uron).tags = NULL;
  (*uron).cron = cron;
  return uron;
}

uron_t * geturon(string_t s) {
  const string_t p = "^([0-9]+)[[:space:]]{1}([^[:space:]]*)[[:space:]]{1}([^\r\n]+)[[:space:]]*$";
  string_t *match;
  int match_c = regmatch(&match, s, p, 4);
  if (match_c != 4) {
    regmatchfree(&match, match_c);
    return NULL;
  }
  uint32_t id;
  sscanf(match[1], "%d", &id);
  const string_t tagx = match[2];
  const string_t cronx = match[3];

  cron_t *cron = getcron(cronx);
  string_t *tags;
  int tag_n = gettags(&tags, tagx);

  regmatchfree(&match, match_c);

  uron_t *uron = malloc(sizeof(uron_t));
  (*uron).id = id;
  (*uron).tag_n = tag_n;
  (*uron).tags = tags;
  (*uron).cron = cron;
  return uron;
}

int fgeturons(uron_t ***urons, FILE *stream) {
  (*urons) = NULL;
  char line[URON_LINE_MAX];
  int uron_c = 0;
  for (;;) {
    if (fgets(line, URON_LINE_MAX, stream) == NULL) {
      break;
    }
    uron_t *uron = geturon(line);
    if (uron == NULL) {
      continue;
    }
    (*urons) =
      (uron_t **)
      xrealloc(
        (*urons),
        sizeof(uron_t *) * (uron_c + 1));
    (*urons)[uron_c] = uron;
    uron_c++;
  }
  return uron_c;
}

int dgeturons(uron_t ***urons) {
  (*urons) = NULL;
  string_t *paths;
  int file_c = getfpaths(&paths, URON_DIR);

  int uron_c = 0;
  int i;
  for (i = 0; i < file_c; i++) {
    string_t path = paths[i];
    FILE *stream = fopen(path, "r");
    if (!stream) {
      fprintf(stderr, "failed: open %s\n", path);
      continue;
    }
    uron_t **_urons;
    int _uron_c = fgeturons(&_urons, stream);
    (*urons) =
      (uron_t **)
      xrealloc(
        (*urons),
        sizeof(uron_t *) * (uron_c + _uron_c));
    memcpy((*urons) + uron_c, _urons, sizeof(uron_t *) * _uron_c);
    free(_urons);
    uron_c += _uron_c;
    fclose(stream);
  }
  free(paths);
  return uron_c;
}

bool ided(const uron_t *uron, const uint32_t *ids, int n) {
  for (int i = 0; i < n; i++) {
    if ((*uron).id == ids[i]) {
      return true;
    }
  }
  return false;
}

bool owned(const uron_t *uron, const string_t username) {
  return strcmp((*(*uron).cron).username, username) == 0;
}

int geturons(uron_t ***urons, const string_t username, const string_t tag, 
    const uint32_t *ids, int n, const string_t cron_dir) {
  cron_t **crons;
  int cron_c = dgetcrons(&crons, cron_dir);
  uron_t **tmp_urons; 
  int tmp_uron_c = dgeturons(&tmp_urons);

  int i, j;
  for (i = 0; i < cron_c; i++) {
    cron_t *cron = crons[i];
    int known = 0;
    for (j = 0; j < tmp_uron_c; j++) {
      uron_t *uron = tmp_urons[j];
      if (eqcron(cron, (*uron).cron)) {
        known = 1;
        break;
      }
    }
    if (!known) {
      uron_t *uron = makeuron(cron);
      saveuron(uron);
      tmp_uron_c++;
      tmp_urons = xrealloc(tmp_urons, sizeof(uron_t *) * tmp_uron_c);
      tmp_urons[tmp_uron_c - 1] = uron;
    }
  }
  (*urons) = xmalloc(sizeof(uron_t *) * tmp_uron_c);
  int uron_c = 0;
  for (i = 0; i < tmp_uron_c; i++) {
    uron_t *uron = tmp_urons[i];
    int alive = 0;
    for (j = 0; j < cron_c; j++) {
      cron_t *cron = crons[j];
      if (eqcron(cron, (*uron).cron)) {
        alive = 1;
        break;
      }
    }
    if (alive &&
        (!username || owned(uron, username)) &&
        (!n || ided(uron, ids, n)) &&
        (!tag || tagged(uron, tag))
        ) {
      (*urons)[uron_c] = uron;
      uron_c++;
    } else {
      freeuron(uron);
    }
  }
  for (i = 0; i < cron_c; i++) {
    /* double free ? */
    // freecron(crons[i]);
  }
  free(crons);
  free(tmp_urons);

  return uron_c;
}
