#ifndef URON_CRON_H
#define URON_CRON_H

#include <stdio.h>

struct cron_struct {
  char *minute;
  char *hour;
  char *day_of_month;
  char *month;
  char *day_of_week;
  char *username;
  char *command;
};

int dgetcrons(struct cron_struct ***, const char *);
int fgetcrons(struct cron_struct ***, FILE *);
struct cron_struct * getcron(const char *);
void freecron(struct cron_struct *);
int eqcron(struct cron_struct *, struct cron_struct *);
int crontox(char **, struct cron_struct *);

#endif /* URON_CRON_H */
