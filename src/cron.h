/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_CRON_H
#define URON_CRON_H

#include "types.h"

#include <stdio.h>

typedef struct cron_struct {
  string minute;
  string hour;
  string day_of_month;
  string month;
  string day_of_week;
  string username;
  string command;
} Cron;

int dgetcrons(Cron ***, cstring);
int fgetcrons(Cron ***, FILE *);
Cron * getcron(cstring);
void freecron(Cron *);
bool eqcron(Cron *, Cron *);
int crontox(string *, Cron *);

#endif /* URON_CRON_H */
