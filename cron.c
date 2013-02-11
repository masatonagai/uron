#include <stdio.h>
#include <stdlib.h>
#include "cron.h"

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
