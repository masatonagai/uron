#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "cron.h"

static int test_dgetcrons() {
  struct cron_struct *actual_crons;
  int cron_c = dgetcrons(&actual_crons, "./dummy_cron.d/");
  int i;
  for (i = 0; i < cron_c; i++) {
    printf(
        "%s %s %s %s %s %s %s\n",
        actual_crons[i].minute,
        actual_crons[i].hour,
        actual_crons[i].day_of_month,
        actual_crons[i].month,
        actual_crons[i].day_of_week,
        actual_crons[i].username,
        actual_crons[i].command);
  }
  free(actual_crons);
  return EXIT_SUCCESS;
}

static int test_fgetcrons() {
  int expect_cronc = 3;
  struct cron_struct expect_crons[] = {
  /* mi   hr    md    mo    wk    usr       cmd */
     "0",  "8",  "*",  "*",  "*", "masato", "echo good morning",
     "0", "12",  "*",  "*",  "*", "masato", "echo good afternoon",
     "0", "22",  "*",  "*",  "*", "masato", "echo good night"
  };

  FILE *stream;
  stream = fopen("./dummy_cron.d/cron1", "r");
  struct cron_struct *actual_crons;
  int actual_cronc = fgetcrons(&actual_crons, stream);
  fclose(stream);
  if (expect_cronc != actual_cronc) {
    fprintf(stderr, "%s:%d failed. expected=%d, actual=%d\n", __FILE__, __LINE__, expect_cronc, actual_cronc);
    return EXIT_FAILURE;
  }

  int i;
  char expect_cronx[LINE_MAX];
  char actual_cronx[LINE_MAX];
  for (i = 0; i < expect_cronc; i++) {
    sprintf(
        expect_cronx,
        "%s %s %s %s %s %s %s",
        expect_crons[i].minute,
        expect_crons[i].hour,
        expect_crons[i].day_of_month,
        expect_crons[i].month,
        expect_crons[i].day_of_week,
        expect_crons[i].username,
        expect_crons[i].command);
    sprintf(
        actual_cronx,
        "%s %s %s %s %s %s %s",
        actual_crons[i].minute,
        actual_crons[i].hour,
        actual_crons[i].day_of_month,
        actual_crons[i].month,
        actual_crons[i].day_of_week,
        actual_crons[i].username,
        actual_crons[i].command);
    if (strcmp(expect_cronx, actual_cronx) != 0) {
      fprintf(stderr, "%s:%d failed. expected=%s, actual=%s\n", __FILE__, __LINE__, expect_cronx, actual_cronx);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

static int test_getcrons() {
  char *cron_xs[] = {
    "0 8 * * mon masato echo good morning",
    "0 12 * * mon masato echo good afternoon",
    "0 22 * * mon masato echo good night"
  };
  int cron_c = 2;
  struct cron_struct *crons;
  getcrons(&crons, cron_xs, 2);

  int i;
  char buff[LINE_MAX];
  for (i = 0; i < cron_c; i++) {
    struct cron_struct cron = *(crons+i);
    sprintf(
        buff,
        "%s %s %s %s %s %s %s",
        cron.minute,
        cron.hour,
        cron.day_of_month,
        cron.month,
        cron.day_of_week,
        cron.username,
        cron.command);
    if (strcmp(buff, cron_xs[i]) != 0) {
      fprintf(stderr, "%s:%d failed. expected=%s, actual=%s\n", __FILE__, __LINE__, *(cron_xs+i), buff);
      free(crons);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  if (test_getcrons() == EXIT_SUCCESS &&
      test_fgetcrons() == EXIT_SUCCESS &&
      test_dgetcrons() == EXIT_SUCCESS) {
    fprintf(stdout, "%s succeeded.\n", __FILE__);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
