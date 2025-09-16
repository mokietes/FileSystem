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

// Helper function to load a directory from disk
dirEntry* loadDirectory(int blockLoc, int size) {
    if (blockLoc < 0) return NULL;
    
    dirEntry* dir = malloc(size * vcb->blockSize);
    if (dir == NULL) return NULL;
    
    if (LBAread(dir, size, blockLoc) != size) {
        free(dir);
        return NULL;
    }
    
    return dir;
}

// Helper function to save a directory to disk
int saveDirectory(dirEntry* dir, int blockLoc, int size) {
    if (dir == NULL || blockLoc < 0) return -1;
    
    return (LBAwrite(dir, size, blockLoc) == size) ? 0 : -1;
}

// Helper function to parse path and find the target directory
dirEntry* parsePath(const char* path) {
    if (path == NULL) return NULL;
    
    // Handle absolute paths
    if (path[0] == '/') {
        path++; // Skip leading slash
    }
    
    // If no path or just "/", return root directory
    if (path[0] == '\0') {
        return rootDir;
    }
    
    // Start from root directory
    dirEntry* currentDir = rootDir;
    
    // Split path by '/' and traverse
    char* pathCopy = strdup(path);
    char* token = strtok(pathCopy, "/");
    
    while (token != NULL) {
        dirEntry* entry = findDirEntry(currentDir, token);
        if (entry == NULL || !entry->isDir) {
            free(pathCopy);
            return NULL; // Path not found or not a directory
        }
        
        // Loads the subdirectory
        int dirSize = (entry->size + vcb->blockSize - 1) / vcb->blockSize;
        dirEntry* subDir = loadDirectory(entry->blockLoc, dirSize);
        if (subDir == NULL) {
            free(pathCopy);
            return NULL;
        }
        
        // If this was a loaded directory (not root), free the previous one
        if (currentDir != rootDir) {
            free(currentDir);
        }
        
        currentDir = subDir;
        token = strtok(NULL, "/");
    }
    
    free(pathCopy);
    return currentDir;
}

// Directory iteration functions

// Opens a directory for reading
fdDir *fs_opendir(const char *pathname) {
    if (!pathname) return NULL;
    dirEntry *targetDir = parsePath(pathname);
    if (!targetDir) return NULL;
    fdDir *dirp = malloc(sizeof(fdDir));
    if (!dirp) {
        if (targetDir != rootDir) free(targetDir);
        return NULL;
    }
    dirp->dirEntryPosition = 0;
    dirp->directory = targetDir;
    dirp->di = malloc(sizeof(struct fs_diriteminfo));
    if (!dirp->di) {
        free(dirp);
        if (targetDir != rootDir) free(targetDir);
        return NULL;
    }
    return dirp;
}
} 