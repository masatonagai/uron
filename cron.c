#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "cron.h"

static char buff[LINE_MAX];

int fgetcrons(struct cron_struct **crons, FILE *stream) {
  (*crons) = NULL;
  int cron_c;
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
    (*cron).minute = malloc(2 + 1);
    (*cron).hour = malloc(2 + 1);
    (*cron).day_of_month = malloc(2 + 1);
    sscanf(
      buff,
      "%s %s %s",
      (*cron).minute,
      (*cron).hour,
      (*cron).day_of_month);
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
    (*cron).minute = malloc(2 + 1);
    (*cron).hour = malloc(2 + 1);
    (*cron).day_of_month = malloc(2 + 1);
    sscanf(cron_x, "%s %s %s", (*cron).minute, (*cron).hour, (*cron).day_of_month);
    (*crons)[i] = (*cron);
  }
}
