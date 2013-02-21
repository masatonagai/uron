#ifndef URON_H
#define URON_H

#include "cron.h"

struct uron_struct {
  unsigned int id;
  size_t tag_n;
  char **tags;
  struct cron_struct *cron;
};

void freeuron(struct uron_struct *);
void saveuron(struct uron_struct *);
struct uron_struct * makeuron(struct cron_struct *);
struct uron_struct * geturon(char *);
int fgeturons(struct uron_struct ***, FILE *);
int dgeturons(struct uron_struct ***);

#endif /* URON_H */
