/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_EXEC_H
#define URON_EXEC_H

#include "types.h"

void exec(const string_t username, const string_t tag, const unsigned int *ids, 
    int n, const string_t cron_dir);

#endif /* URON_EXEC_H */
