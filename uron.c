#include "uron.h"

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
#include "list.h"

enum command {
  help_command, list_command
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
