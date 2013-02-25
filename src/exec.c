#include "exec.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include "uron.h"

static void run_command(const struct uron_struct *uron) {
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

void exec(const char *tag, const unsigned int *ids, int n, const char *cron_dir) {
  struct uron_struct **urons;
  int uron_c = geturons(&urons, tag, ids, n, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    run_command(urons[i]);
  }
}