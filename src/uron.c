#include "uron.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "cron.h"
#include "path.h"
#include "util.h"
#include "term.h"
#include "io.h"
#include "list.h"
#include "tag.h"
#include "exec.h"

#define URON_LINE_MAX 512 /* includes newline character */
#define URON_LINE_PADCHAR '\r'
#define URON_ID_MIN 1

enum command {
  help_command, list_command, tag_command, untag_command,
  exec_command
};

void freeuron(struct uron_struct *uron) {
  freecron((*uron).cron);
  free(uron);
}

static struct uron_struct * getlasturon(int fd) {
  off_t eof = lseek(fd, 0, SEEK_END);
  if (eof > 0) {
    off_t o = lseek(fd, (off_t) (eof - URON_LINE_MAX), SEEK_SET);
    if (o >= 0) {
      char *luronx = (char *) xmalloc(URON_LINE_MAX);
      read(fd, luronx, URON_LINE_MAX);
      struct uron_struct *luron = geturon(luronx);
      free(luronx);
      return luron;
    }
  }
  return NULL;
}

void saveuron(struct uron_struct *uron) {
  /*
  printf("open %s\n", URON_DIR);
  DIR *uron_dir = opendir(URON_DIR);
  if (uron_dir == NULL) {
    fprintf(stderr, "failed to open \"%s\"\n", URON_DIR);
    exit(EXIT_FAILURE);
  }
  */
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/%s", URON_DIR, "1");

  int fd = open(path, O_RDWR | O_CREAT | O_SYNC);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  int add = !(*uron).id;
  if (add) {
    (*uron).id = URON_ID_MIN;
    struct uron_struct *luron = getlasturon(fd);
    if (luron != NULL) {
      (*uron).id = (*luron).id + 1;
      free(luron);
    }
  } else {
    lseek(fd, (off_t) (URON_LINE_MAX * ((*uron).id - URON_ID_MIN)), SEEK_SET);
  }

  char *cronx, *tagx;
  crontox(&cronx, (*uron).cron);
  tagstox(&tagx, (const char **) (*uron).tags, (*uron).tag_n);

  char line[URON_LINE_MAX + 1];
  int len = snprintf(line, sizeof(line), "%d %s %s", (*uron).id, tagx, cronx);

  free(cronx);
  free(tagx);

  // right pad
  int i, n;
  for (i = len, n = URON_LINE_MAX - 1; i < n; i++) {
    line[i] = URON_LINE_PADCHAR;
  }
  line[URON_LINE_MAX - 1] = '\n';
  line[URON_LINE_MAX] = '\0';

  ssize_t wb = write(fd, line, strlen(line));
  if (wb == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
}

struct uron_struct * makeuron(struct cron_struct *cron) {
  struct uron_struct *uron = (struct uron_struct *) xmalloc(sizeof(struct uron_struct));
  (*uron).id = 0;
  (*uron).tag_n = 0;
  (*uron).tags = NULL;
  (*uron).cron = cron;
  return uron;
}

struct uron_struct * geturon(char *s) {
  const char *p = "^([0-9]+)[[:space:]]{1}([^[:space:]]*)[[:space:]]{1}([^\r\n]+)[[:space:]]*$";
  char **match;
  int match_c = regmatch(&match, s, p, 4);
  if (match_c != 4) {
    return NULL;
  }
  unsigned int id;
  sscanf(match[1], "%d", &id);
  const char *tagx = match[2];
  const char *cronx = match[3];

  struct cron_struct *cron = getcron(cronx);
  char **tags;
  int tag_n = gettags(&tags, tagx);

  struct uron_struct *uron = malloc(sizeof(struct uron_struct));
  (*uron).id = id;
  (*uron).tag_n = tag_n;
  (*uron).tags = tags;
  (*uron).cron = cron;
  return uron;
}

int fgeturons(struct uron_struct ***urons, FILE *stream) {
  (*urons) = NULL;
  char line[URON_LINE_MAX];
  int uron_c = 0;
  for (;;) {
    if (fgets(line, URON_LINE_MAX, stream) == NULL) {
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

int ided(const struct uron_struct *uron, const unsigned int *ids, int n) {
  for (int i = 0; i < n; i++) {
    if ((*uron).id == ids[i]) {
      return 1;
    }
  }
  return 0;
}

int geturons(struct uron_struct ***urons, const char *tag, 
    const unsigned int *ids, int n, const char *cron_dir) {
  struct cron_struct **crons;
  int cron_c = dgetcrons(&crons, cron_dir);
  struct uron_struct **tmp_urons; 
  int tmp_uron_c = dgeturons(&tmp_urons);

  int i, j;
  for (i = 0; i < cron_c; i++) {
    struct cron_struct *cron = crons[i];
    int known = 0;
    for (j = 0; j < tmp_uron_c; j++) {
      struct uron_struct *uron = tmp_urons[j];
      if (eqcron(cron, (*uron).cron)) {
        known = 1;
        break;
      }
    }
    if (!known) {
      struct uron_struct *uron = makeuron(cron);
      saveuron(uron);
      tmp_uron_c++;
      tmp_urons = xrealloc(tmp_urons, sizeof(struct uron_struct *) * tmp_uron_c);
      tmp_urons[tmp_uron_c - 1] = uron;
    }
  }
  (*urons) = xmalloc(sizeof(struct uron_struct *) * tmp_uron_c);
  int uron_c = 0;
  for (i = 0; i < tmp_uron_c; i++) {
    struct uron_struct *uron = tmp_urons[i];
    int alive = 0;
    for (j = 0; j < cron_c; j++) {
      struct cron_struct *cron = crons[j];
      if (eqcron(cron, (*uron).cron)) {
        alive = 1;
        break;
      }
    }
    if (alive &&
        (!n || ided(uron, ids, n)) &&
        (!tag || tagged(uron, tag))
        ) {
      (*urons)[uron_c] = uron;
      uron_c++;
    } else {
      freeuron(uron);
    }
  }
  for (i = 0; i < cron_c; i++) {
    /* double free ? */
    // freecron(crons[i]);
  }
  free(crons);
  free(tmp_urons);

  return uron_c;
}


static void help() {
  fprintf(stderr,
    "usage: uron [option(s)] [id(s)]\n"
    "  commands:\n"
    "    -h, --help     show this help\n"
    "    -l, --list     list jobs\n"
    "    -a, --add      add tag to job\n"
    "    -r, --remove   remove tag from job\n"
    "    -x, --exec     execute job command\n"
    "  command modifiers:\n"
    "    -t, --tag      tag\n"
    "    -d, --dir      cron dir (default is \"%s\")\n",
    CRON_DIR
    );
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  struct option long_opts[] = {
    { "help",   no_argument,        0, 'h' },
    { "list",   no_argument,        0, 'l' },
    { "add",    no_argument,        0, 'a' },
    { "remove", no_argument,        0, 'r' },
    { "exec",   no_argument,        0, 'x' },
    { "tag",    required_argument,  0, 't' },
    { "dir",    required_argument,  0, 'd' }
  };

  enum command cmd = help_command;
  char *cron_dir = CRON_DIR;
  char *tag = NULL;
  for (;;) {
    int index;
    int c = getopt_long(argc, argv, "hlarxd:t:", long_opts, &index);
    if (c == -1) {
      break;
    }
    switch (c) {
      /* commands */
      case 'l':
        cmd = list_command;
        break;
      case 'a':
        cmd = tag_command;
        break;
      case 'r':
        cmd = untag_command;
        break;
      case 'x':
        cmd = exec_command;
        break;
      case 'h':
        cmd = help_command;
        break;
      /* command modifiers */
      case 'd':
        cron_dir = optarg;
        break;
      case 't':
        tag = optarg;
        break;
    }
  }
  int n = argc - optind;
  unsigned int *uron_ids =
    (unsigned int *) xmalloc(sizeof(unsigned int) * n);
  if (n > 0) {
    int i, j;
    unsigned int tmpid;
    for (i = optind, j = 0; i < argc; i++, j++) {
      sscanf(argv[i], "%u", &tmpid);
      uron_ids[j] = tmpid;
    }
  }

  switch (cmd) {
    case help_command:
      help();
      break;
    case tag_command:
      addtag(tag, uron_ids, n);
      break;
    case untag_command:
      rmtag(tag, uron_ids, n);
      break;
    case list_command:
      list(tag, uron_ids, n, cron_dir);
      break;
    case exec_command:
      exec(tag, uron_ids, n, cron_dir);
      break;
  }

  return EXIT_SUCCESS;
}
