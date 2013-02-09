#include "cron.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char **argv) {
  char *cron_xs[] = {
    "10 * *", "0 12 31"
  };
  struct cron_struct *crons;
  int cron_c = get_crons(&crons, cron_xs);
  if (cron_c != 2) {
    fprintf(stderr, "%s failed. expected=%d, actual=%d\n", *argv, 2, cron_c);
    free(crons);
    return EXIT_FAILURE;
  }
  int i;
  char buff[LINE_MAX];
  for (i = 0; i < cron_c; i++) {
    struct cron_struct cron = *(crons+i);
    sprintf(buff, "%s %s %s", cron.minute, cron.hour, cron.day_of_month);
    if (strcmp(buff, cron_xs[i]) != 0) {
      fprintf(stderr, "%s failed. expected=%s, actual=%s\n", *argv, *(cron_xs+i), buff);
      free(crons);
      return EXIT_FAILURE;
    }
  }
  fprintf(stdout, "%s succeeded.\n", *argv);
  return EXIT_SUCCESS;
}

