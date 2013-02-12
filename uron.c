#include <stdio.h>
#include <stdlib.h>
#include "cron.h"

int main(int argc, char **argv) {
  struct cron_struct *crons;
  int cron_c = dgetcrons(&crons, "/etc/cron.d/");
  int i;
  printf(
      "%2s  %2s  %2s  %3s  %3s  %10s  %30s\n",
      "i", "h", "d", "m", "w", "u", "c");
  for (i = 0; i < cron_c; i++) {
    printf(
      "%2s  %2s  %2s  %3s  %3s  %10s  %30s\n",
      crons[i].minute,
      crons[i].hour,
      crons[i].day_of_month,
      crons[i].month,
      crons[i].day_of_week,
      crons[i].username,
      crons[i].command);
  }
}
