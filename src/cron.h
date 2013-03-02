/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_CRON_H
#define URON_CRON_H

#include "types.h"

#include <stdio.h>

struct cron_struct {
  string minute;
  string hour;
  string day_of_month;
  string month;
  string day_of_week;
  string username;
  string command;
};

int dgetcrons(struct cron_struct ***, cstring);
int fgetcrons(struct cron_struct ***, FILE *);
struct cron_struct * getcron(cstring);
void freecron(struct cron_struct *);
int eqcron(struct cron_struct *, struct cron_struct *);
int crontox(string *, struct cron_struct *);

#endif /* URON_CRON_H */
