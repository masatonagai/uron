#include "exec.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include "uron.h"

static void run_command(const char *username, const char *command) {
  struct passwd p, *result;
  char buff[sysconf(_SC_GETPW_R_SIZE_MAX)];
  if (getpwnam_r(username, &p, buff, sizeof(buff), &result) != 0) {
    perror("getpwnam_r");
  } else {
    printf("name:%s, id:%ld\n", p.pw_name, (long int) p.pw_uid);
    printf("command=%s", command);
    setuid(p.pw_uid);
    // TODO unescape '%'
    // http://linux.die.net/man/3/system
    system(command);
  }
}

void exec(const char *tag, const char *cron_dir) {
  struct uron_struct **urons;
  int uron_c = geturons(&urons, tag, cron_dir);
  for (int i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    run_command((*cron).username, (*cron).command);
  }
}
