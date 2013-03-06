/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_CRON_H
#define URON_CRON_H

#include "types.h"

#include <stdio.h>

typedef struct cron_s {
  string_t minute;
  string_t hour;
  string_t day_of_month;
  string_t month;
  string_t day_of_week;
  string_t username;
  string_t command;
} cron_t;

int dgetcrons(cron_t ***, const string_t);
int fgetcrons(cron_t ***, FILE *);
cron_t * getcron(const string_t);
void freecron(cron_t *);
bool eqcron(cron_t *, cron_t *);
int crontox(string_t *, cron_t *);

#endif /* URON_CRON_H */
