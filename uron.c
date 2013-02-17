#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include <getopt.h>
#include "cron.h"
#include "path.h"
#include "util.h"

#define H_NO "NO"
#define H_MIN "MIN"
#define H_HR "HR"
#define H_DOM "DOM"
#define H_MON "MON"
#define H_DOW "DOW"
#define H_USR "USR"
#define H_CMD "CMD"

enum command {
  help_command, list_command
};

struct uron_struct {
  unsigned int n;
  struct cron_struct *cron;
};

void freeuron(struct uron_struct *uron) {
  freecron((*uron).cron);
  free(uron);
}

struct uron_struct * makeuron(unsigned int n, struct cron_struct *cron) {
  struct uron_struct *uron = (struct uron_struct *) xmalloc(sizeof(struct uron_struct));
  (*uron).n = n;
  (*uron).cron = cron;
  return uron;
}

static void list(char *cron_dir) {
  struct cron_struct **crons;
  int cron_c = dgetcrons(&crons, cron_dir);
  int i;
  struct uron_struct **urons =
    (struct uron_struct **) xmalloc(sizeof(struct uron_struct *) * cron_c);
  for (i = 0; i < cron_c; i++) {
    struct uron_struct *uron = makeuron(i, crons[i]);
    urons[i] = uron;
  }
  free(crons);

  int h_no_len = MIN(2, strlen(H_NO));
  int h_min_len = strlen(H_MIN);
  int h_hr_len = strlen(H_HR);
  int h_dom_len = strlen(H_DOM);
  int h_mon_len = strlen(H_MON);
  int h_dow_len = strlen(H_DOW);
  int h_usr_len = strlen(H_USR);
  int h_cmd_len = strlen(H_CMD);
  for (i = 0; i < cron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    h_min_len = MAX(h_min_len, strlen((*cron).minute));
    h_hr_len = MAX(h_hr_len, strlen((*cron).hour));
    h_dom_len = MAX(h_dom_len, strlen((*cron).day_of_month));
    h_mon_len = MAX(h_mon_len, strlen((*cron).month));
    h_dow_len = MAX(h_dow_len, strlen((*cron).day_of_week));
    h_usr_len = MAX(h_usr_len, strlen((*cron).username));
    h_cmd_len = MAX(h_cmd_len, strlen((*cron).command));
  }
  char buff[LINE_MAX];
  snprintf(buff, sizeof(buff),
      "\e[30;47;2m%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\e[0m\n",
      h_no_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  printf(buff, H_NO, H_MIN, H_HR, H_DOM, H_MON, H_DOW, H_USR, H_CMD);
  snprintf(buff, sizeof(buff),
      "%%-%dd %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\n",
      h_no_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  for (i = 0; i < cron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    printf(
        buff,
        (*uron).n,
        (*cron).minute,
        (*cron).hour,
        (*cron).day_of_month,
        (*cron).month,
        (*cron).day_of_week,
        (*cron).username,
        (*cron).command);
    freeuron(uron);
  }
  free(urons);
  exit(EXIT_SUCCESS);
}

static void help() {
  fprintf(stderr, "usage: uron [OPTION...]\n");
  fprintf(stderr, "  commands:\n");
  fprintf(stderr, "    -h, --help  show this help\n");
  fprintf(stderr, "    -l, --list  list jobs up\n");
  fprintf(stderr, "  command modifiers:\n");
  fprintf(stderr, "    -d, --dir   specify cron dir (default is \"%s\")\n", CRON_DIR);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  struct option long_opts[] = {
    { "help", no_argument, 0, 'h' },
    { "list", no_argument, 0, 'l' },
    { "dir",  required_argument, 0, 'd' }
  };

  enum command cmd = help_command;
  char *cron_dir = CRON_DIR;
  for (;;) {
    int index;
    int c = getopt_long(argc, argv, "hld:0", long_opts, &index);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 'l':
        cmd = list_command;
        break;
      case 'h':
        cmd = help_command;
        break;
      case 'd':
        cron_dir = optarg;
        break;
    }
  }

  switch (cmd) {
    case help_command:
      help();
      break;
    case list_command:
      list(cron_dir);
      break;
  }

  return EXIT_SUCCESS;
}
