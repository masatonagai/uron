#ifndef CRON_H
#define CRON_H

struct cron_struct {
  char *minute;
  char *hour;
  char *day_of_month;
  char *month;
  char *day_of_week;
  char *username;
  char *command;
};

int dgetcrons(struct cron_struct **, char *dirname);
int fgetcrons(struct cron_struct **, FILE *stream);
void getcrons(struct cron_struct **, char **, int);

#endif /* CRON_H */
