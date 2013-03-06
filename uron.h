/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_H
#define URON_H

#include "types.h"
#include "cron.h"

typedef struct uron_s {
  uint32_t id;
  size_t tag_n;
  string_t *tags;
  cron_t *cron;
} uron_t;

void freeuron(uron_t *);
void saveuron(uron_t *);
uron_t * makeuron(cron_t *);
uron_t * geturon(string_t);
int fgeturons(uron_t ***, FILE *);
int dgeturons(uron_t ***urons);
int geturons(uron_t ***urons, const string_t username, const string_t tag, 
    const uint32_t *ids, int n, const string_t cron_dir);

#endif /* URON_H */
