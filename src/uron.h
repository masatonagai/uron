/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_H
#define URON_H

#include "types.h"
#include "cron.h"

typedef struct uron_struct {
  unsigned int id;
  size_t tag_n;
  string *tags;
  Cron *cron;
} Uron;

void freeuron(Uron *);
void saveuron(Uron *);
Uron * makeuron(Cron *);
Uron * geturon(string);
int fgeturons(Uron ***, FILE *);
int dgeturons(Uron ***urons);
int geturons(Uron ***urons, cstring username, cstring tag, 
    const unsigned int *ids, int n, cstring cron_dir);

#endif /* URON_H */
