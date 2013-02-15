#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>
#include "cron.h"

#define N_MATCH 8
#define MIN "[0-9*/,-]+"
#define HR  "[0-9*/,-]+"
#define DOM "[0-9*/,-LW]+"
#define MON "[0-9a-zA-Z*/,-]+"
// #define YR "[0-9*/,-]+"
#define DOW "[0-6a-zA-Z*/,-L#]+"
#define USR "[a-z][-a-z0-9]*"
#define CMD "[^\r\n]+"
#define SP "[[:space:]]"

struct cron_struct * getcron(const char *s) {
  regex_t regex;
  regmatch_t pmatch[N_MATCH];
  char pattern[256];
  sprintf(pattern,
      "^%s*(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s*$",
        SP,MIN,SP,HR, SP,DOM,SP,MON,SP,DOW,SP,USR,SP,CMD,SP
      );
  if (regcomp(&regex, pattern, REG_EXTENDED) != REG_NOERROR) {
    fprintf(stderr, "faild to compile regex. pattern=%s\n", pattern);
    exit(EXIT_FAILURE);
  }
  if (regexec(&regex, s, (size_t) N_MATCH, pmatch, 0) != REG_NOERROR) {
    return NULL;
  }

  char *match[N_MATCH];
  int match_c;
  for (match_c = 0; match_c < N_MATCH; match_c++) {
    regmatch_t m = pmatch[match_c];
    if (m.rm_so == -1) {
      break;
    }
    size_t len = m.rm_eo - m.rm_so;
    match[match_c] = (char *) malloc(len + 1);
    strncpy(match[match_c], s + m.rm_so, len);
    match[match_c][len] = '\0';
  }
  if (match_c != N_MATCH) {
    return NULL;
  }
  struct cron_struct *cron =
    (struct cron_struct *) malloc(sizeof(struct cron_struct));
  (*cron).minute = strdup(match[1]);
  (*cron).hour = strdup(match[2]);
  (*cron).day_of_month = strdup(match[3]);
  (*cron).month = strdup(match[4]);
  (*cron).day_of_week = strdup(match[5]);
  (*cron).username = strdup(match[6]);
  (*cron).command = strdup(match[7]);
  return cron;
}

int dgetcrons(struct cron_struct **crons, char *dirname) {
  (*crons) = NULL;
  struct dirent *entry;
  DIR *dir = opendir(dirname);
  int cron_c = 0;
  while ((entry = readdir(dir)) != NULL) {
    if ((*entry).d_type == DT_REG) {
      const char *filename = (*entry).d_name;
      if (*filename != '.') {
        char path[PATH_MAX];
        strcpy(path, dirname);
        strcpy(path + strlen(dirname), filename);
        FILE *stream = fopen(path, "r");
        if (!stream) {
          fprintf(stderr, "failed: open %s\n", path);
          continue;
        }
        struct cron_struct *_crons;
        int _cron_c = fgetcrons(&_crons, stream);
        (*crons) =
          (struct cron_struct *)
          realloc(
            (*crons),
            sizeof(struct cron_struct) * (cron_c + _cron_c));
        memcpy((*crons) + cron_c, _crons, sizeof(struct cron_struct) * _cron_c);
        cron_c += _cron_c;
        fclose(stream);
      }
    }
  }
  closedir(dir);
  return cron_c;
}

int fgetcrons(struct cron_struct **crons, FILE *stream) {
  (*crons) = NULL;
  char buff[LINE_MAX];
  int cron_c = 0;
  for (;;) {
    if (fgets(buff, LINE_MAX, stream) == NULL) {
      break;
    }
    struct cron_struct *cron = getcron(buff);
    if (cron == NULL) {
      continue;
    }
    (*crons) =
      (struct cron_struct *)
      realloc(
        (*crons),
        sizeof(struct cron_struct) * (cron_c + 1));
    (*crons)[cron_c] = (*cron);
    cron_c++;
  }
  return cron_c;
}
