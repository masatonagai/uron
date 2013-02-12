#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "cron.h"

static char buff[LINE_MAX];

int fgetcrons(struct cron_struct **crons, FILE *stream) {
  (*crons) = NULL;
  int cron_c = 0;
  for (;;) {
    if (fgets(buff, LINE_MAX, stream) == NULL) {
      break;
    }
    (*crons) =
      (struct cron_struct *)
      realloc(
        (*crons),
        sizeof(struct cron_struct) * (cron_c + 1));
    struct cron_struct *cron = &(*crons)[cron_c];
    (*cron).minute = malloc(2 + 1);           // 0-59
    (*cron).hour = malloc(2 + 1);             // 0-23
    (*cron).day_of_month = malloc(2 + 1);     // 1-31
    (*cron).month = malloc(3 + 1);            // 1-12 or jan-dec
    (*cron).day_of_week = malloc(3 + 1);      // 0-6 or sun-sat
    (*cron).username = malloc(_SC_LOGIN_NAME_MAX);
    (*cron).command = malloc(LINE_MAX / 2);
    sscanf(
      buff,
      "%s %s %s %s %s %s %[^\n]",
      (*cron).minute,
      (*cron).hour,
      (*cron).day_of_month,
      (*cron).month,
      (*cron).day_of_week,
      (*cron).username,
      (*cron).command);
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
