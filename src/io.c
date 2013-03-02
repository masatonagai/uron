/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "util.h"

void freefpaths(char ***fpaths) {
  free(fpaths);
}

int getfpaths(char ***paths, char *dirname) {
  (*paths) = NULL;
  int file_c = 0;
  DIR *dir = opendir(dirname);
  if (dir != NULL) {
    struct dirent *entry;
    char path[PATH_MAX];
    while ((entry = readdir(dir)) != NULL) {
      if ((*entry).d_type == DT_REG) {
        const char *filename = (*entry).d_name;
        if (*filename != '.') {
          file_c++;
          snprintf(path, PATH_MAX, "%s/%s", dirname, filename);
          (*paths) = xrealloc(*paths, sizeof(char *) * file_c);
          (*paths)[file_c - 1] = path;
        }
      }
    }
    closedir(dir);
  }
  return file_c;
}

