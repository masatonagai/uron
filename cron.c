#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "cron.h"


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
    const char *line;
    if (fgets(buff, LINE_MAX, stream) == NULL) {
      break;
    }
    char c[2];
    sscanf(buff, "%s", c);
    if (c[0] == '#') {
      continue;
    }

    struct cron_struct *cron = malloc(sizeof(struct cron_struct));
    (*cron).minute = malloc(2 + 1);           // 0-59
    (*cron).hour = malloc(2 + 1);             // 0-23
    (*cron).day_of_month = malloc(2 + 1);     // 1-31
    (*cron).month = malloc(3 + 1);            // 1-12 or jan-dec
    (*cron).day_of_week = malloc(3 + 1);      // 0-6 or sun-sat
    (*cron).username = malloc(_SC_LOGIN_NAME_MAX);
    (*cron).command = malloc(LINE_MAX / 2);
    int scanned = sscanf(
      buff,
      "%s %s %s %s %s %s %[^\n]",
      (*cron).minute,
      (*cron).hour,
      (*cron).day_of_month,
      (*cron).month,
      (*cron).day_of_week,
      (*cron).username,
      (*cron).command);
    if (scanned != 7) {
      free(cron);
      continue;
    }
    (*crons) =
      (struct cron_struct *)
      realloc(
        (*crons),
        sizeof(struct cron_struct) * (cron_c + 1));
    (*crons)[cron_c] = *cron;
    cron_c++;
  }
  return cron_c;
}

void getcrons(struct cron_struct **crons, char **cron_xs, int cron_c) {
  (*crons) = (struct cron_struct *) calloc(cron_c, sizeof(struct cron_struct));
  int i;
  for (i = 0; i < cron_c; i++) {
    char *cron_x = *(cron_xs + i);
    struct cron_struct *cron = (struct cron_struct *) malloc(sizeof(struct cron_struct));
    (*cron).minute = malloc(2 + 1);           // 0-59
    (*cron).hour = malloc(2 + 1);             // 0-23
    (*cron).day_of_month = malloc(2 + 1);     // 1-31
    (*cron).month = malloc(3 + 1);            // 1-12 or jan-dec
    (*cron).day_of_week = malloc(3 + 1);      // 0-6 or sun-sat
    (*cron).username = malloc(_SC_LOGIN_NAME_MAX);
    (*cron).command = malloc(LINE_MAX / 2);
    sscanf(
      cron_x,
      "%s %s %s %s %s %s %[^\n]",
      (*cron).minute,
      (*cron).hour,
      (*cron).day_of_month,
      (*cron).month,
      (*cron).day_of_week,
      (*cron).username,
      (*cron).command);
    (*crons)[i] = (*cron);
  }
}
