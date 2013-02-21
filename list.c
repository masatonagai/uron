#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "uron.h"
#include "cron.h"
#include "util.h"
#include "term.h"
#include "tag.h"

#define H_ID "ID"
#define H_TAG "TAG"
#define H_MIN "MIN"
#define H_HR "HR"
#define H_DOM "DOM"
#define H_MON "MON"
#define H_DOW "DOW"
#define H_USR "USR"
#define H_CMD "CMD"


void list(const char *tag, char *cron_dir) {
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
      struct uron_struct *uron = makeuron(cron);
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
    if (alive && (!tag || tagged(uron, tag))) {
      aurons[auron_c] = uron;
      auron_c++;
    } else {
      freeuron(uron);
    }
  }
  for (i = 0; i < cron_c; i++) {
    /* double free ? */
    // freecron(crons[i]);
  }
  free(crons);
  free(urons);
  uron_c = auron_c;
  urons = aurons;

  printf("TOTAL: %d\n", uron_c);

  if (!uron_c) {
    exit(EXIT_SUCCESS);
  }

  int h_id_len = strlen(H_ID);
  int h_tag_len = strlen(H_TAG);
  int h_min_len = strlen(H_MIN);
  int h_hr_len = strlen(H_HR);
  int h_dom_len = strlen(H_DOM);
  int h_mon_len = strlen(H_MON);
  int h_dow_len = strlen(H_DOW);
  int h_usr_len = strlen(H_USR);
  int h_cmd_len = strlen(H_CMD);
  char id[11 + 1];
  for (i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    char *tagx;
    h_id_len = MAX(h_id_len, sprintf(id, "%u", (*uron).id));
    h_tag_len = MAX(h_tag_len, tagstox(&tagx, (const char **) (*uron).tags, (*uron).tag_n));
    free(tagx);
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
      "\e[7m%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\e[0m\n",
      h_id_len, h_tag_len,
      h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  if (buff[chars - 1] != '\n') {
    buff[chars - 1] = '\n';
  }
  printf(buff, H_ID, H_TAG, H_MIN, H_HR, H_DOM, H_MON, H_DOW, H_USR, H_CMD);
  chars = snprintf(buff, buff_size,
      "%%-%dd %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds\n",
      h_id_len, h_tag_len,
      h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len);
  if (buff[chars - 1] != '\n') {
    buff[chars - 1] = '\n';
  }
  for (i = 0; i < uron_c; i++) {
    struct uron_struct *uron = urons[i];
    struct cron_struct *cron = (*uron).cron;
    char *tagx;
    tagstox(&tagx, (const char **) (*uron).tags, (*uron).tag_n);
    printf(
        buff,
        (*uron).id,
        tagx,
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
