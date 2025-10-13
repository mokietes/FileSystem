#ifndef FS_UTILITY_H
#define FS_UTILITY_H

#include <stddef.h>
#include "mfs.h"

fdDir * fs_opendir(const char *pathname);

int fs_closedir(fdDir *dirp);

char *fs_getcwd(char *buf, size_t size);

// Return 1 if path is a file, 0 otherwise
int fs_isFile(char *filename);

// Return 1 if path is a directory, 0 otherwise
int fs_isDir(char *pathname);

#endif
