/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "uron.h"
#include "types.h"
#include "path.h"
#include "util.h"
#include "list.h"
#include "tag.h"
#include "exec.h"

#include <stdlib.h>
#include <getopt.h>

typedef enum command_e {
  help_command, list_command, add_command, remove_command, exec_command
} command_t;

static void help() {
  fprintf(stderr,
    "usage: uron [option(s)] [id(s)]\n"
    "  commands:\n"
    "    -h, --help         show this help\n"
    "    -l, --list         list jobs\n"
    "    -a, --add=tag      add the tag to job\n"
    "    -r, --remove=tag   remove the tag from job\n"
    "    -x, --exec         execute job command\n"
    "  command modifiers:\n"
    "    -u, --user=user    select jobs of the user\n"
    "    -t, --tag=tag      select jobs have the tag\n"
    "    -n, --no-tags      select jobs do not have any tags\n"
    "    -d, --dir          switch the cron dir (default is \"%s\")\n",
    CRON_DIR
    );
  exit(EXIT_FAILURE);
}

int main(int argc, string_t *argv) {
  struct option long_opts[] = {
    { "help",    no_argument,        0, 'h' },
    { "list",    no_argument,        0, 'l' },
    { "add",     required_argument,  0, 'a' },
    { "remove",  required_argument,  0, 'r' },
    { "exec",    no_argument,        0, 'x' },
    { "user",    required_argument,  0, 'u' },
    { "tag",     required_argument,  0, 't' },
    { "no-tags", no_argument,        0, 'n' },
    { "dir",     required_argument,  0, 'd' }
  };

  command_t cmd = help_command;
  string_t cron_dir = CRON_DIR;
  string_t tag_for_write = NULL;
  string_t tag_for_read = NULL;
  string_t username = NULL;
  for (;;) {
    int index;
    int c = getopt_long(argc, argv, "hlxa:r:d:u:t:n", long_opts, &index);
    if (c == -1) {
      break;
    }
    switch (c) {
      /* commands */
      case 'l':
        cmd = list_command;
        break;
      case 'a':
        cmd = add_command;
        tag_for_write = optarg;
        break;
      case 'r':
        cmd = remove_command;
        tag_for_write = optarg;
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
      case 'u':
        username = optarg;
        break;
      case 't':
        tag_for_read = optarg;
        break;
      case 'n':
        tag_for_read = URON_NO_TAGS;
        break;
    }
  }
  int n = argc - optind;
  uint32_t *uron_ids =
    (uint32_t *) xmalloc(sizeof(uint32_t) * n);
  if (n > 0) {
    int i, j;
    uint32_t tmpid;
    for (i = optind, j = 0; i < argc; i++, j++) {
      sscanf(argv[i], "%u", &tmpid);
      uron_ids[j] = tmpid;
    }
  }

  switch (cmd) {
    case help_command:
      help();
      break;
    case add_command:
      addtag(tag_for_write, username, tag_for_read, uron_ids, n, cron_dir);
      break;
    case remove_command:
      rmtag(tag_for_write, username, tag_for_read, uron_ids, n, cron_dir);
      break;
    case list_command:
      list(username, tag_for_read, uron_ids, n, cron_dir);
      break;
    case exec_command:
      exec(username, tag_for_read, uron_ids, n, cron_dir);
      break;
  }

  return EXIT_SUCCESS;
}
