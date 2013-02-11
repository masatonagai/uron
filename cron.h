struct cron_struct {
  char *minute;
  char *hour;
  char *day_of_month;
};

void getcrons(struct cron_struct **, char **, int);
