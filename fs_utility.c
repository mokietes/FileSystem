#define _DEFAULT_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parsePath.h"
#include "mfs.h"

#define DIRMAX_LEN 4096

/*
 * Opens a directory given a pathname.
 * Loads the directory block from disk and prepares for iteration.
 * Returns a directory handle (fdDir pointer), or NULL on error.
 */
