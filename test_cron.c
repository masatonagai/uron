#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "cron.h"

static int test_getcrons() {
  char *cron_xs[] = {
    "10 * *", "0 12 31"
  };
  int cron_c = 2;
  struct cron_struct *crons;
  getcrons(&crons, cron_xs, 2);

  int i;
  char buff[LINE_MAX];
  for (i = 0; i < cron_c; i++) {
    struct cron_struct cron = *(crons+i);
    sprintf(buff, "%s %s %s", cron.minute, cron.hour, cron.day_of_month);
    if (strcmp(buff, cron_xs[i]) != 0) {
      fprintf(stderr, "%s failed. expected=%s, actual=%s\n", __FILE__, *(cron_xs+i), buff);
      free(crons);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  if (test_getcrons() == EXIT_SUCCESS) {
    fprintf(stdout, "%s succeeded.\n", __FILE__);
  }
}
