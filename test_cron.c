#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "cron.h"

static char *crontox(const struct cron_struct cron) {
  char *cronx = (char *) malloc(LINE_MAX);
  sprintf(
      cronx,
      "%s %s %s %s %s %s %s",
      cron.minute,
      cron.hour,
      cron.day_of_month,
      cron.month,
      cron.day_of_week,
      cron.username,
      cron.command);
  return cronx;
}

static int test_getcron() {
  const struct cron_struct expect_cron = {
    "*/5", "10-12", "5L", "mon-fri", "JAN,FEB", "masato", "echo hello" };
  const char *expect_cronx = crontox(expect_cron);

  const struct cron_struct actual_cron = (*getcron(expect_cronx));
  char *actual_cronx = crontox(actual_cron);

  if (strcmp(expect_cronx, actual_cronx) != 0) {
    fprintf(stderr, "%s:%d failed. expected=%s, actual=%s\n", __FILE__, __LINE__, expect_cronx, actual_cronx);
    return 0;
  }
  return 1;
}

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
    return 0;
  }

  int i;
  for (i = 0; i < expect_cronc; i++) {
    const char *expect_cronx = crontox(expect_crons[i]);
    const char *actual_cronx = crontox(actual_crons[i]);
    printf("%c <=> %c\n", expect_cronx[strlen(expect_cronx)-1], actual_cronx[strlen(actual_cronx)-1]);
    if (strcmp(expect_cronx, actual_cronx) != 0) {
      fprintf(stderr, "%s:%d failed. expected=%s, actual=%s\n", __FILE__, __LINE__, expect_cronx, actual_cronx);
      return 0;
    }
  }
  return 1;
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
  if (test_getcron() & test_fgetcrons()) {
    fprintf(stdout, "%s succeeded.\n", __FILE__);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
