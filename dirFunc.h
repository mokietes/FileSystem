**************************************************************/

#ifndef DIR_FUNC_H
#define DIR_FUNC_H

#include <sys/types.h>
#include "dirEntry.h"

#define CHANGE_DIR_FACTOR 2


// Finds and returns a pointer to the first available empty directory entry
// Calls function to expand directory if directory is full
dirEntry * findFreeDirEntry(dirEntry *de);

// Expands or shrinks the passed in directory to support the new passed in number of
// directory entries in the passed in directory
// returns the new number of entries in the changed directory, or -1 if no change was made
// this could be greater than the passed in new number of entries
// NOT intended to be used for the root directory
