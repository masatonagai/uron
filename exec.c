/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "exec.h"
#include "uron.h"
#include "types.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

static void run_command(const uron_t *uron) {
  struct passwd p, *result;
  char buff[sysconf(_SC_GETPW_R_SIZE_MAX)];
  if (getpwnam_r((*(*uron).cron).username, &p, buff, sizeof(buff), &result) != 0) {
    perror("getpwnam_r");
  } else {
    printf("%u %s %s\n", (*uron).id, p.pw_name, (*(*uron).cron).command);
    setuid(p.pw_uid);
    system((*(*uron).cron).command);
  }
}

void exec(const string_t username, const string_t tag, const uint32_t *ids,
    int n, const string_t cron_dir) {
  uron_t **urons;
  int uron_c = geturons(&urons, username, tag, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    run_command(urons[i]);
  }
}
