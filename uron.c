#include <stdio.h>
#include <stdlib.h>
#include "cron.h"

int main(int argc, char **argv) {
  struct cron_struct *crons;
  int cron_c = dgetcrons(&crons, "/etc/cron.d/");
  int i;
  printf(
      "%4s  %4s  %4s  %4s  %4s  %4s  %10s  %s\n",
      "NO", "MI", "HR", "DM", "MO", "DW", "USR", "CMD");
  for (i = 0; i < cron_c; i++) {
    printf(
      "%4d  %4s  %4s  %4s  %4s  %4s  %10s  %s\n",
      i,
      crons[i].minute,
      crons[i].hour,
      crons[i].day_of_month,
      crons[i].month,
      crons[i].day_of_week,
      crons[i].username,
      crons[i].command);
  }
}
