/*
 * Copyright (C) 2013 Masato Nagai
 *
 */

#ifndef URON_EXEC_H
#define URON_EXEC_H

#include "types.h"

void exec(cstring username, cstring tag, const unsigned int *ids, 
    int n, cstring cron_dir);

#endif /* URON_EXEC_H */
