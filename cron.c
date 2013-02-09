#include <stdio.h>
#include <stdlib.h>
#include "cron.h"

int get_crons(struct cron_struct **, char **);
static struct cron_struct get_cron(char *);

int get_crons(struct cron_struct **crons, char **cron_xs) {
  char *cron_x;
  int cron_c = 0;
  for (;;) {
    cron_x = *(cron_xs+cron_c);
    if (!cron_x) break;
    (*crons) = (struct cron_struct *)
      realloc((*crons), (cron_c+1)*sizeof(struct cron_struct));
    if (!crons) {
      fprintf(stderr, "realloc");
      return -1;
    }
    *((*crons)+cron_c) = get_cron(cron_x);
    cron_c++;
  }
  return cron_c;
}

static struct cron_struct get_cron(char *x) {
  struct cron_struct *cron = malloc(sizeof(struct cron_struct));
  (*cron).minute = malloc(2+1);
  (*cron).hour = malloc(2+1);
  (*cron).day_of_month = malloc(2+1);
  sscanf(x, "%s %s %s", (*cron).minute, (*cron).hour, (*cron).day_of_month);
  return *cron;
}

