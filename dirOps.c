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

// Reads the next entry in the directory
struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    if (!dirp || !dirp->directory) return NULL;
    int entries = dirp->directory[0].size / sizeof(dirEntry);
    while (dirp->dirEntryPosition < entries) {
        dirEntry *entry = &dirp->directory[dirp->dirEntryPosition];
        dirp->dirEntryPosition++;
        // skips empty, . and ..
        if (entry->name[0] != '\0' && strcmp(entry->name, ".") != 0 && strcmp(entry->name, "..") != 0) {
            dirp->di->d_reclen = sizeof(struct fs_diriteminfo);
            dirp->di->fileType = entry->isDir ? FT_DIRECTORY : FT_REGFILE;
            strncpy(dirp->di->d_name, entry->name, 255);
            dirp->di->d_name[255] = '\0';
            return dirp->di;
        }
    }
    return NULL;
}

// Closes the directory iterator and free memory
int fs_closedir(fdDir *dirp) {
    if (!dirp) return -1;
    if (dirp->di) free(dirp->di);
    if (dirp->directory && dirp->directory != rootDir) free(dirp->directory);
    free(dirp);
    return 0;
}

// Path utility functions

char* fs_getcwd(char* pathname, size_t size) {
    if (pathname == NULL || size == 0) return NULL;
    
    strncpy(pathname, currentWorkingDir, size - 1);
    pathname[size - 1] = '\0';
    
    return pathname;
}

int fs_setcwd(char* pathname) {
    if (pathname == NULL) return -1;
    
    // Checks if the path exists and is a directory
    dirEntry* targetDir = parsePath(pathname);
    if (targetDir == NULL) {
        return -1; // Path not found
    }
    
    // Frees the loaded directory if it's not root
    if (targetDir != rootDir) {
        free(targetDir);
    }
    
    // Updates current working directory
    if (pathname[0] == '/') {
        strncpy(currentWorkingDir, pathname, sizeof(currentWorkingDir) - 1);
    } else {
        // Relative path - append to current directory
        if (strcmp(currentWorkingDir, "/") != 0) {
            strncat(currentWorkingDir, "/", sizeof(currentWorkingDir) - strlen(currentWorkingDir) - 1);
        }
        strncat(currentWorkingDir, pathname, sizeof(currentWorkingDir) - strlen(currentWorkingDir) - 1);
    }
    
    currentWorkingDir[sizeof(currentWorkingDir) - 1] = '\0';
    
    return 0;
}

// Directory utility functions

int fs_isFile(char* filename) {
    if (filename == NULL) return 0;
    
    // Parses the path to find the file
    dirEntry* parentDir = parsePath(filename);
    if (parentDir == NULL) return 0;
    
    // Extracts the filename from the path
    char* lastSlash = strrchr(filename, '/');
    char* name = (lastSlash != NULL) ? lastSlash + 1 : filename;
    
    // Finds the file in the parent directory
    dirEntry* entry = findDirEntry(parentDir, name);
    
    // Frees the loaded directory if it's not root
    if (parentDir != rootDir) {
        free(parentDir);
    }
    
    return (entry != NULL && !entry->isDir) ? 1 : 0;
}

int fs_isDir(char* pathname) {
    if (pathname == NULL) return 0;
    
    // Parses the path to find the directory
    dirEntry* targetDir = parsePath(pathname);
    if (targetDir == NULL) return 0;
    
    // Frees the loaded directory if it's not root
    if (targetDir != rootDir) {
        free(targetDir);
    }
    
    return 1; // If parsePath succeeded, it's a directory
}

int fs_mkdir(const char* pathname, mode_t mode) {
    if (pathname == NULL) return -1;
    
    // Extracts parent directory path and new directory name
    char* pathCopy = strdup(pathname);
    char* lastSlash = strrchr(pathCopy, '/');
    
    char* parentPath = NULL;
    char* dirName = NULL;
    
    if (lastSlash == NULL) {
        // No slash - create in current directory
        parentPath = strdup(".");
        dirName = pathCopy;
    } else {
        // Splits path into parent and name
        *lastSlash = '\0';
        parentPath = pathCopy;
        dirName = lastSlash + 1;
    }
    
    // Loads parent directory
    dirEntry* parentDir = parsePath(parentPath);
    if (parentDir == NULL) {
        free(pathCopy);
        return -1; // Parent directory not found
    }
    
    // Checks if directory already exists
    if (findDirEntry(parentDir, dirName) != NULL) {
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1; // Directory already exists
    }
    
    // Creates new directory
    dirEntry* newDir = createDir(DEFAULT_ENTRIES, parentDir);
    if (newDir == NULL) {
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1;
    }
    
    // Finds an empty slot in parent directory
    int entries = parentDir[0].size / sizeof(dirEntry);
    int emptySlot = -1;
    
    for (int i = 2; i < entries; i++) {
        if (parentDir[i].name[0] == '\0') {
            emptySlot = i;
            break;
        }
    }
    
    if (emptySlot == -1) {
        // No empty slots - need to expand directory
        // This is a simplified implementation - in practice you'd need to handle directory expansion
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(newDir);
        free(pathCopy);
        return -1;
    }
    
    // Adds the new directory entry to parent
    strncpy(parentDir[emptySlot].name, dirName, MAX_NAME);
    parentDir[emptySlot].name[MAX_NAME] = '\0';
    parentDir[emptySlot].blockLoc = newDir[0].blockLoc;
    parentDir[emptySlot].size = newDir[0].size;
    parentDir[emptySlot].isDir = 1;
    parentDir[emptySlot].createTime = time(NULL);
    parentDir[emptySlot].modifyTime = parentDir[emptySlot].createTime;
    parentDir[emptySlot].accessTime = parentDir[emptySlot].createTime;
    
    // Saves parent directory
    if (parentDir == rootDir) {
        saveRootDir();
    } else {
        int parentSize = (parentDir[0].size + vcb->blockSize - 1) / vcb->blockSize;
        saveDirectory(parentDir, parentDir[0].blockLoc, parentSize);
        free(parentDir);
    }
    
    free(newDir);
    free(pathCopy);
    
    return 0;
}

int fs_rmdir(const char* pathname) {
    if (pathname == NULL) return -1;
    
    // Extracts parent directory path and directory name
    char* pathCopy = strdup(pathname);
    char* lastSlash = strrchr(pathCopy, '/');
    
    char* parentPath = NULL;
    char* dirName = NULL;
    
    if (lastSlash == NULL) {
        parentPath = strdup(".");
        dirName = pathCopy;
    } else {
        *lastSlash = '\0';
        parentPath = pathCopy;
        dirName = lastSlash + 1;
    }
    
    // Loads parent directory
    dirEntry* parentDir = parsePath(parentPath);
    if (parentDir == NULL) {
        free(pathCopy);
        return -1;
    }
    
    // Finds the directory to remove
    dirEntry* targetEntry = findDirEntry(parentDir, dirName);
    if (targetEntry == NULL || !targetEntry->isDir) {
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1; // Directory not found or not a directory
    }
    
    // Loads the target directory to check if it's empty
    int targetSize = (targetEntry->size + vcb->blockSize - 1) / vcb->blockSize;
    dirEntry* targetDir = loadDirectory(targetEntry->blockLoc, targetSize);
    if (targetDir == NULL) {
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1;
    }
    
    // Checks if directory is empty (only . and .. entries)
    int entries = targetDir[0].size / sizeof(dirEntry);
    int isEmpty = 1;
    
    for (int i = 2; i < entries; i++) {
        if (targetDir[i].name[0] != '\0') {
            isEmpty = 0;
            break;
        }
    }
    
    if (!isEmpty) {
        free(targetDir);
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1; // Directory not empty
    }
    
    // Removes the directory entry from parent
    int parentEntries = parentDir[0].size / sizeof(dirEntry);
    for (int i = 2; i < parentEntries; i++) {
        if (strcmp(parentDir[i].name, dirName) == 0) {
            // Clear the entry
            parentDir[i].name[0] = '\0';
            parentDir[i].blockLoc = -1;
            parentDir[i].isDir = 0;
            parentDir[i].size = 0;
            break;
        }
    }
    
    // Frees the target directory blocks
    freeBlocks(targetEntry->blockLoc, targetSize);
    
    // Saves parent directory
    if (parentDir == rootDir) {
        saveRootDir();
    } else {
        int parentSize = (parentDir[0].size + vcb->blockSize - 1) / vcb->blockSize;
        saveDirectory(parentDir, parentDir[0].blockLoc, parentSize);
        free(parentDir);
    }
    
    free(targetDir);
    free(pathCopy);
    
    return 0;
}

int fs_delete(char* filename) {
    if (filename == NULL) return -1;
    
    // Extracts parent directory path and filename
    char* pathCopy = strdup(filename);
    char* lastSlash = strrchr(pathCopy, '/');
    
    char* parentPath = NULL;
    char* fileName = NULL;
    
    if (lastSlash == NULL) {
        parentPath = strdup(".");
        fileName = pathCopy;
    } else {
        *lastSlash = '\0';
        parentPath = pathCopy;
        fileName = lastSlash + 1;
    }
    
    // Loads parent directory
    dirEntry* parentDir = parsePath(parentPath);
    if (parentDir == NULL) {
        free(pathCopy);
        return -1;
    }
    
    // Finds the file to delete
    dirEntry* targetEntry = findDirEntry(parentDir, fileName);
    if (targetEntry == NULL || targetEntry->isDir) {
        if (parentDir != rootDir) {
            free(parentDir);
        }
        free(pathCopy);
        return -1; // File not found or is a directory
    }
    
} 