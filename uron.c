#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include "cron.h"

#define H_NO "NO"
#define H_MIN "MIN"
#define H_HR "HR"
#define H_DOM "DOM"
#define H_MON "MON"
#define H_DOW "DOW"
#define H_USR "USR"
#define H_CMD "CMD"

int main(int argc, char **argv) {
  struct cron_struct *crons;
  int cron_c = dgetcrons(&crons, "/etc/cron.d/");
  int i;

  int h_no_len = MIN(2, strlen(H_NO));
  int h_min_len = strlen(H_MIN);
  int h_hr_len = strlen(H_HR);
  int h_dom_len = strlen(H_DOM);
  int h_mon_len = strlen(H_MON);
  int h_dow_len = strlen(H_DOW);
  int h_usr_len = strlen(H_USR);
  int h_cmd_len = strlen(H_CMD);
  for (i = 0; i < cron_c; i++) {
    h_min_len = MAX(h_min_len, strlen(crons[i].minute));
    h_hr_len = MAX(h_hr_len, strlen(crons[i].hour));
    h_dom_len = MAX(h_dom_len, strlen(crons[i].day_of_month));
    h_mon_len = MAX(h_mon_len, strlen(crons[i].month));
    h_dow_len = MAX(h_dow_len, strlen(crons[i].day_of_week));
    h_usr_len = MAX(h_usr_len, strlen(crons[i].username));
    h_cmd_len = MAX(h_cmd_len, strlen(crons[i].command));
  }
  char buff[LINE_MAX];
  sprintf(buff,
      "\e[30;47;2m%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\e[0m\n",
      h_no_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  printf(buff, H_NO, H_MIN, H_HR, H_DOM, H_MON, H_DOW, H_USR, H_CMD);
  sprintf(buff,
      "%%-%dd %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\n",
      h_no_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  for (i = 0; i < cron_c; i++) {
    printf(
        buff,
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
