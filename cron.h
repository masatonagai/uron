struct cron_struct {
  char *minute;
  char *hour;
  char *day_of_month;
};

int get_crons(struct cron_struct **, char **);
