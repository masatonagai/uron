#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "cron.h"

static int test_fgetcrons() {
  int expect_cronc = 2;
  struct cron_struct expect_crons[] = {
    "10", "*", "*",
    "0", "23", "31"
  };

  FILE *stream;
  stream = fopen("dummy_cron", "r");
  struct cron_struct *actual_crons;
  int actual_cronc = fgetcrons(&actual_crons, stream);
  fclose(stream);

  if (expect_cronc != actual_cronc) {
    fprintf(stderr, "%s failed. expected=%d, actual=%d\n", __FILE__, expect_cronc, actual_cronc);
    return EXIT_FAILURE;
  }

  int i;
  char expect_cronx[LINE_MAX];
  char actual_cronx[LINE_MAX];
  for (i = 0; i < expect_cronc; i++) {
    sscanf(expect_cronx, "%s %s %s",
        expect_crons[i].minute,
        expect_crons[i].hour,
        expect_crons[i].day_of_month);
    sscanf(actual_cronx, "%s %s %s",
        actual_crons[i].minute,
        actual_crons[i].hour,
        actual_crons[i].day_of_month);
    if (strcmp(expect_cronx, actual_cronx) != 0) {
      fprintf(stderr, "%s failed. expected=%s, actual=%s\n", __FILE__, expect_cronx, actual_cronx);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

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
  if (test_getcrons() == EXIT_SUCCESS && test_fgetcrons() == EXIT_SUCCESS) {
    fprintf(stdout, "%s succeeded.\n", __FILE__);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
