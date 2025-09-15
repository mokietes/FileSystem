/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: dirOps.c
*
* Description:: Implements directory operations including iteration,
* path utilities, and directory management functions for the custom
* file system. This includes fs_opendir, fs_readdir, fs_closedir,
* fs_getcwd, fs_setcwd, fs_isFile, fs_isDir, fs_mkdir, fs_rmdir,
* and fs_delete functions.
*
**************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "mfs.h"
#include "dirEntry.h"
#include "fsLow.h"
#include "vcb.h"
#include "freeSpace.h"

#define DEFAULT_ENTRIES 50

// Global current working directory
static char currentWorkingDir[256] = "/";

// Helper function to find a directory entry by name
dirEntry* findDirEntry(dirEntry* dir, const char* name) {
    if (dir == NULL || name == NULL) return NULL;
    
    int entries = dir[0].size / sizeof(dirEntry);
    for (int i = 0; i < entries; i++) {
        if (dir[i].name[0] != '\0' && strcmp(dir[i].name, name) == 0) {
            return &dir[i];
        }
    }
    return NULL;
}

