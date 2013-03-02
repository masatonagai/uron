/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_EXEC_H
#define URON_EXEC_H

#include "types.h"

void exec(const string username, const string tag, const unsigned int *ids, 
    int n, const string cron_dir);

#endif /* URON_EXEC_H */
