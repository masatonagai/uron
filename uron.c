#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include "cron.h"
#include "path.h"
#include "util.h"
#include "term.h"
#include "io.h"

#define H_ID "ID"
#define H_MIN "MIN"
#define H_HR "HR"
#define H_DOM "DOM"
#define H_MON "MON"
#define H_DOW "DOW"
#define H_USR "USR"
#define H_CMD "CMD"


enum command;
struct uron_struct;

void freeuron(struct uron_struct *);
void saveuron(struct uron_struct *);
struct uron_struct * makeuron(unsigned int, struct cron_struct *);
struct uron_struct * geturon(char *);
int fgeturons(struct uron_struct ***, FILE *);
int dgeturons(struct uron_struct ***);


enum command {
  help_command, list_command
};

struct uron_struct {
  unsigned int id;
  struct cron_struct *cron;
};

void freeuron(struct uron_struct *uron) {
  freecron((*uron).cron);
  free(uron);
}

void saveuron(struct uron_struct *uron) {
  DIR *uron_dir = opendir(URON_DIR);
  if (uron_dir == NULL) {
    fprintf(stderr, "failed to open \"%s\"\n", URON_DIR);
    exit(EXIT_FAILURE);
  }
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", URON_DIR, "1");
  FILE *stream = fopen(path, "a+");
  if (stream == NULL) {
    fprintf(stderr, "failed to open \"%s\"\n", path);
    exit(EXIT_FAILURE);
  }
  flockfile(stream);

  fseek(stream, 0, SEEK_END);
  char line[1024];
  int i;
  for (i = 0; fseek(stream, -2, SEEK_CUR) == 0; i++) {
    char ch = fgetc(stream);
    if (ch == '\n') {
      break;
    }
    line[i] = ch;
  }
  if (i > 0) {
    line[i] = '\0';
    char *luronx = xmalloc(i);
    int j, k;
    for (j = 0, k= (i - 1); j < i; j++, k--) {
      luronx[j] = line[k];
    }
    struct uron_struct *luron = geturon(luronx);
    if (luron != NULL) {
      (*uron).id = (*luron).id + 1;
    }
    fseek(stream, 0, SEEK_END);
  }

  char *cronx;
  crontox(&cronx, (*uron).cron);
  if (fprintf(stream, "%d %s\n", (*uron).id, cronx) < 0) {
    fprintf(stderr, "failed to write to \"%s\"\n", path);
  }
  funlockfile(stream);
  fclose(stream);
}

struct uron_struct * makeuron(unsigned int id, struct cron_struct *cron) {
  struct uron_struct *uron = (struct uron_struct *) xmalloc(sizeof(struct uron_struct));
  (*uron).id = id;
  (*uron).cron = cron;
  return uron;
}

struct uron_struct * geturon(char *s) {
  unsigned int id;
  char cron_x[LINE_MAX]; // todo
  sscanf(s, "%d %[^\n]", &id, cron_x);
  struct cron_struct *cron = getcron(cron_x);
  struct uron_struct *uron = malloc(sizeof(struct uron_struct));
  (*uron).id = id;
  (*uron).cron = cron;
  return uron;
}

int fgeturons(struct uron_struct ***urons, FILE *stream) {
  (*urons) = NULL;
  char line[LINE_MAX];
  int uron_c = 0;
  for (;;) {
    if (fgets(line, LINE_MAX, stream) == NULL) {
      break;
    }
    struct uron_struct *uron = geturon(line);
    if (uron == NULL) {
      continue;
    }
    (*urons) =
      (struct uron_struct **)
      xrealloc(
        (*urons),
        sizeof(struct uron_struct *) * (uron_c + 1));
    (*urons)[uron_c] = uron;
    uron_c++;
  }
  return uron_c;
}

int dgeturons(struct uron_struct ***urons) {
  (*urons) = NULL;
  char **paths;
  int file_c = getfpaths(&paths, URON_DIR);

  int uron_c = 0;
  int i;
  for (i = 0; i < file_c; i++) {
    char *path = paths[i];
    FILE *stream = fopen(path, "r");
    if (!stream) {
      fprintf(stderr, "failed: open %s\n", path);
      continue;
    }
    struct uron_struct **_urons;
    int _uron_c = fgeturons(&_urons, stream);
    (*urons) =
      (struct uron_struct **)
      xrealloc(
        (*urons),
        sizeof(struct uron_struct *) * (uron_c + _uron_c));
    memcpy((*urons) + uron_c, _urons, sizeof(struct uron_struct *) * _uron_c);
    free(_urons);
    uron_c += _uron_c;
    fclose(stream);
  }
  free(paths);
  return uron_c;
}

static void list(char *cron_dir) {
  struct cron_struct **crons;
  int cron_c = dgetcrons(&crons, cron_dir);
  struct uron_struct **urons;
  int uron_c = dgeturons(&urons);
  int i, j;
  for (i = 0; i < cron_c; i++) {
    struct cron_struct *cron = crons[i];
    int known = 0;
    for (j = 0; j < uron_c; j++) {
      struct uron_struct *uron = urons[j];
      if (eqcron(cron, (*uron).cron)) {
        known = 1;
        break;
      }
    }
    if (!known) {
      struct uron_struct *uron = makeuron(0, cron);
      saveuron(uron);
      uron_c++;
      urons = xrealloc(urons, sizeof(struct uron_struct *) * uron_c);
      urons[uron_c - 1] = uron;
    }
  }
  struct uron_struct **aurons = xmalloc(sizeof(struct uron_struct *) * uron_c);
  int auron_c = 0;
  for (i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    int alive = 0;
    for (j = 0; j < cron_c; j++) {
      struct cron_struct *cron = crons[j];
      if (eqcron(cron, (*uron).cron)) {
        alive = 1;
        break;
      }
    }
    if (alive) {
      aurons[auron_c] = uron;
      auron_c++;
    } else {
      freeuron(uron);
    }
  }
  for (i = 0; i < cron_c; i++) {
    freecron(crons[i]);
  }
  free(crons);
  free(urons);
  urons = aurons;

  int h_id_len = strlen(H_ID);
  int h_min_len = strlen(H_MIN);
  int h_hr_len = strlen(H_HR);
  int h_dom_len = strlen(H_DOM);
  int h_mon_len = strlen(H_MON);
  int h_dow_len = strlen(H_DOW);
  int h_usr_len = strlen(H_USR);
  int h_cmd_len = strlen(H_CMD);
  char tmps[12];
  for (i = 0; i < cron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    sprintf(tmps, "%u", (*uron).id);
    h_id_len = MAX(h_id_len, strlen(tmps));
    h_min_len = MAX(h_min_len, strlen((*cron).minute));
    h_hr_len = MAX(h_hr_len, strlen((*cron).hour));
    h_dom_len = MAX(h_dom_len, strlen((*cron).day_of_month));
    h_mon_len = MAX(h_mon_len, strlen((*cron).month));
    h_dow_len = MAX(h_dow_len, strlen((*cron).day_of_week));
    h_usr_len = MAX(h_usr_len, strlen((*cron).username));
    h_cmd_len = MAX(h_cmd_len, strlen((*cron).command));
  }

  struct term_struct term = terminfo();
  int buff_size = term.columns;
  char *buff = xmalloc(buff_size);
  int chars;
  chars = snprintf(buff, buff_size,
      "\e[7m%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\e[0m\n",
      h_id_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  if (buff[chars - 1] != '\n') {
    buff[chars - 1] = '\n';
  }
  printf(buff, H_ID, H_MIN, H_HR, H_DOM, H_MON, H_DOW, H_USR, H_CMD);
  chars = snprintf(buff, buff_size,
      "\e[0m%%-%dd\e[0m %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\n",
      h_id_len, h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  if (buff[chars - 1] != '\n') {
    buff[chars - 1] = '\n';
  }
  for (i = 0; i < cron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    printf(
        buff,
        (*uron).id,
        (*cron).minute,
        (*cron).hour,
        (*cron).day_of_month,
        (*cron).month,
        (*cron).day_of_week,
        (*cron).username,
        (*cron).command);
    freeuron(uron);
  }
  free(buff);
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
