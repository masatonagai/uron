#ifndef URON_H
#define URON_H

#include "cron.h"

struct uron_struct {
  unsigned int id;
  struct cron_struct *cron;
};

void freeuron(struct uron_struct *);
void saveuron(struct uron_struct *);
struct uron_struct * makeuron(unsigned int, struct cron_struct *);
struct uron_struct * geturon(char *);
int fgeturons(struct uron_struct ***, FILE *);
int dgeturons(struct uron_struct ***);

#endif /* URON_H */
