/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "cron.h"
#include "types.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>

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

int crontox(string *cronx, Cron *cron) {
  (*cronx) = (string) xmalloc(LINE_MAX);
  return snprintf(
      (*cronx),
      LINE_MAX,
      "%s %s %s %s %s %s %s",
      (*cron).minute,
      (*cron).hour,
      (*cron).day_of_month,
      (*cron).month,
      (*cron).day_of_week,
      (*cron).username,
      (*cron).command);
}

bool eqcron(Cron *a, Cron *b) {
  if (a == b) {
    return true;
  }
  if (strcmp((*a).minute, (*b).minute) == 0 &&
      strcmp((*a).hour, (*b).hour) == 0 &&
      strcmp((*a).day_of_month, (*b).day_of_month) == 0 &&
      strcmp((*a).month, (*b).month) == 0 &&
      strcmp((*a).day_of_week, (*b).day_of_week) == 0 &&
      strcmp((*a).username, (*b).username) == 0 &&
      strcmp((*a).command, (*b).command) == 0) {
    return true;
  }
  return false;
}

void freecron(Cron *cron) {
  free((*cron).minute);
  free((*cron).hour);
  free((*cron).day_of_month);
  free((*cron).month);
  free((*cron).day_of_week);
  free((*cron).username);
  free((*cron).command);
  free(cron);
}

static Cron * makecron(
    string minute, string hour, string day_of_month, string month, string day_of_week,
    string username, string command) {
  Cron *cron = xmalloc(sizeof(Cron));
  (*cron).minute = strdup(minute);
  (*cron).hour = strdup(hour);
  (*cron).day_of_month = strdup(day_of_month);
  (*cron).month = strdup(month);
  (*cron).day_of_week = strdup(day_of_week);
  (*cron).username = strdup(username);
  (*cron).command = strdup(command);
  return cron;
}

static int unescape_command(string unescaped, cstring escaped) {
  int unescaped_len = 0;
  for (int i = 0, n = strlen(escaped); i < n; i++) {
    // quote from man of crontab(5):
    //   Percent-signs  (%)  in  the  command, unless escaped with backslash (\),
    //   will be changed into newline characters"
    if (i < (n - 1) && escaped[i] == '\\' && escaped[i + 1] == '%') {
      continue;
    }
    unescaped[unescaped_len++] = escaped[i];
  }
  unescaped[unescaped_len] = '\0';
  return unescaped_len;
}

Cron * getcron(cstring s) {
  regex_t regex;
  regmatch_t pmatch[N_MATCH];
  char pattern[256];
  snprintf(pattern, sizeof(pattern),
      "^%s*(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s+(%s)%s*$",
        SP,MIN,SP,HR, SP,DOM,SP,MON,SP,DOW,SP,USR,SP,CMD,SP
      );
  if (regcomp(&regex, pattern, REG_EXTENDED) != REG_NOERROR) {
    fprintf(stderr, "faild to compile regex. pattern=%s\n", pattern);
    exit(EXIT_FAILURE);
  }
  if (regexec(&regex, s, (size_t) N_MATCH, pmatch, 0) != REG_NOERROR) {
    regfree(&regex);
    return NULL;
  }
  regfree(&regex);

  string match[N_MATCH];
  int match_c;
  for (match_c = 0; match_c < N_MATCH; match_c++) {
    regmatch_t m = pmatch[match_c];
    if (m.rm_so == -1) {
      break;
    }
    size_t len = m.rm_eo - m.rm_so;
    match[match_c] = (string) xmalloc(len + 1);
    strncpy(match[match_c], s + m.rm_so, len);
    match[match_c][len] = '\0';
  }
  if (match_c != N_MATCH) {
    return NULL;
  }
  char command[strlen(match[7]) + 1];
  unescape_command(command, match[7]);
  Cron *cron = makecron(
      match[1], match[2], match[3], match[4], match[5],
      match[6], command);
  int i;
  for (i = 0; i < match_c; i++) {
    free(match[i]);
  }
  return cron;
}

int dgetcrons(Cron ***crons, cstring dirname) {
  (*crons) = NULL;
  int cron_c = 0;

  DIR *dir = opendir(dirname);
  struct dirent *entry;
  char path[PATH_MAX];
  while ((entry = readdir(dir)) != NULL) {
    if ((*entry).d_type == DT_REG || (*entry).d_type == DT_LNK) {
      cstring filename = (*entry).d_name;
      if (*filename != '.') {
        snprintf(path, PATH_MAX, "%s/%s", dirname, filename);
        FILE *stream = fopen(path, "r");
        if (!stream) {
          fprintf(stderr, "failed: open %s\n", path);
          continue;
        }
        Cron **_crons;
        int _cron_c = fgetcrons(&_crons, stream);
        (*crons) =
          (Cron **)
          xrealloc(
            (*crons),
            sizeof(Cron *) * (cron_c + _cron_c));
        memcpy((*crons) + cron_c, _crons, sizeof(Cron *) * _cron_c);
        free(_crons);
        cron_c += _cron_c;
        fclose(stream);
      }
    }
  }
  closedir(dir);
  return cron_c;
}

int fgetcrons(Cron ***crons, FILE *stream) {
  (*crons) = NULL;
  char line[LINE_MAX];
  int cron_c = 0;
  for (;;) {
    if (fgets(line, LINE_MAX, stream) == NULL) {
      break;
    }
    Cron *cron = getcron(line);
    if (cron == NULL) {
      continue;
    }
    (*crons) =
      (Cron **)
      xrealloc(
        (*crons),
        sizeof(Cron *) * (cron_c + 1));
    (*crons)[cron_c] = cron;
    cron_c++;
  }
  return cron_c;
}
