/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "list.h"
#include "uron.h"
#include "types.h"
#include "types.h"
#include "cron.h"
#include "util.h"
#include "term.h"
#include "tag.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define URON_COLUMNS 9
#define H_ID "ID"
#define H_TAG "TAG"
#define H_MIN "MIN"
#define H_HR "HR"
#define H_DOM "DOM"
#define H_MON "MON"
#define H_DOW "DOW"
#define H_USR "USR"
#define H_CMD "CMD"


void list(const string_t username, const string_t tag, const unsigned int *ids, 
    int n, const string_t cron_dir) {
  uron_t **urons;
  int uron_c = geturons(&urons, username, tag, ids, n, cron_dir);

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
  char id[11 + 1];
  int i;
  for (i = 0; i < uron_c; i++) {
    uron_t *uron = urons[i];
    cron_t *cron = (*uron).cron;
    string_t tagx;
    h_id_len = MAX(h_id_len, sprintf(id, "%u", (*uron).id));
    h_tag_len = MAX(h_tag_len, tagstox(&tagx, (const string_t *) (*uron).tags, (*uron).tag_n));
    free(tagx);
    h_min_len = MAX(h_min_len, strlen((*cron).minute));
    h_hr_len = MAX(h_hr_len, strlen((*cron).hour));
    h_dom_len = MAX(h_dom_len, strlen((*cron).day_of_month));
    h_mon_len = MAX(h_mon_len, strlen((*cron).month));
    h_dow_len = MAX(h_dow_len, strlen((*cron).day_of_week));
    h_usr_len = MAX(h_usr_len, strlen((*cron).username));
  }
  struct term_struct term = terminfo();
  int h_cmd_len = term.columns - 
    (h_id_len + h_tag_len + h_min_len + h_hr_len + h_dom_len + h_mon_len + 
     h_dow_len + h_usr_len + (URON_COLUMNS - 1));

  const string_t const header_prefix = "\e[7m";
  const string_t const header_suffix = "\e[0m";
  const int header_buff_size = term.columns + strlen(header_prefix) + strlen(header_suffix) + 1;
  char header_buff[header_buff_size];
  int chars;
  snprintf(header_buff, header_buff_size,
      "%s%%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds%s",
      header_prefix,
      h_id_len, h_tag_len,
      h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len, h_cmd_len,
      header_suffix);
  const string_t const header_format = strdup(header_buff);
  chars = snprintf(header_buff, header_buff_size, header_format, H_ID, H_TAG, H_MIN, H_HR, H_DOM, H_MON, H_DOW, H_USR, H_CMD);
  puts(header_buff);

  const string_t const row_prefix = "\e[0m";
  const string_t const row_suffix = "\e[0m";
  const int row_buff_size = term.columns + strlen(row_prefix) + strlen(row_suffix);
  char row_buff[row_buff_size];
  snprintf(row_buff, row_buff_size,
      "%s%%-%dd %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-%ds %%-s%s",
      row_prefix,
      h_id_len, h_tag_len,
      h_min_len, h_hr_len, h_dom_len, h_mon_len, h_dow_len,
      h_usr_len,
      row_suffix);
  char command[h_cmd_len + 1];
  const string_t const row_format = strdup(row_buff);
  for (i = 0; i < uron_c; i++) {
    uron_t *uron = urons[i];
    cron_t *cron = (*uron).cron;
    string_t tagx;
    tagstox(&tagx, (const string_t *) (*uron).tags, (*uron).tag_n);
    strncpy(command, (*cron).command, sizeof(command) - 1);
    command[sizeof(command) - 1] = '\0';
    chars = snprintf(
        row_buff,
        row_buff_size,
        row_format,
        (*uron).id,
        tagx,
        (*cron).minute,
        (*cron).hour,
        (*cron).day_of_month,
        (*cron).month,
        (*cron).day_of_week,
        (*cron).username,
        command);
    puts(row_buff);
    freeuron(uron);
  }
  free(urons);
  exit(EXIT_SUCCESS);
}
