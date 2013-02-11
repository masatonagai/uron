#ifndef CRON_H
#define CRON_H

struct cron_struct {
  char *minute;
  char *hour;
  char *day_of_month;
};

int fgetcrons(struct cron_struct **, FILE *stream);
void getcrons(struct cron_struct **, char **, int);

#endif /* CRON_H */
