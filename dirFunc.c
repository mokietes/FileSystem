/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: dirFunc.c
*
* Description:: This file contains implementations for the
* fs_mkdir and fs_rmdir functions as well as related helper
* functions.
*
**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "mfs.h"
#include "fsLow.h"
#include "dirFunc.h"
#include "parsePath.h"
#include "vcb.h"
#include "freeSpace.h"
#include "fs_utility.h"

int fs_mkdir(const char *pathname, mode_t mode) {
    ppInfo ppi;
    int ppRet;

    char *pathCopy = strdup(pathname);
    if (pathCopy == NULL) return -1;

    ppRet = parsePath(pathCopy, &ppi);

    // Check if path is valid
    if (ppRet != 0) return -1;

    // Check if last element of the path already exists in its parent
    if (ppi.index != -1) return -2;

    // Create new directory in its parent
    dirEntry *newDir = createDir(DEFAULT_ENTRIES, ppi.parent);

    // Link the new directory to its parent through
    // a new directory entry it its parent
    dirEntry *newEntry = findFreeDirEntry(ppi.parent);

    newEntry->size = newDir[0].size;
    newEntry->createTime = newDir[0].createTime;
    newEntry->modifyTime = newDir[0].modifyTime;
    newEntry->accessTime = newDir[0].accessTime;
    newEntry->blockLoc = newDir[0].blockLoc;
    newEntry->isDir = newDir[0].isDir;
    strncpy(newEntry->name, ppi.lastElement, MAX_NAME);

    // Save directory to disk
    saveDir(ppi.parent);

    free(newDir);
    newDir = NULL;
    safeFree(ppi.parent); // NOT ROOT OR CWD
    ppi.parent = NULL;

    return 0;
}

int fs_rmdir(const char *pathname) {
    ppInfo ppi;
    int ppRet;

    char *pathCopy = strdup(pathname);
    if (pathCopy == NULL) return -1;

    ppRet = parsePath(pathCopy, &ppi);

    // Check if path is valid
    if (ppRet != 0) return -1;

    // Check if last element of the path doesn't exist in the parent
    if (ppi.index == -1) return -1;

    // Check if it is the root
    if (ppi.index == -2) return -1; 

    // Check if last element of the path is a directory
    if (fs_isDir(pathCopy) != 1) {
        safeFree(ppi.parent);
        ppi.parent = NULL;
        return -1;
    }

    dirEntry *thisDir = loadDir(&ppi.parent[ppi.index]);
    if (thisDir == NULL) {
        safeFree(ppi.parent);
        ppi.parent = NULL;
        return -1;
    }

    // Check if the directory is empty
    if (isDirEmpty(thisDir) != 1) {
        free(thisDir);
        thisDir = NULL;
        safeFree(ppi.parent);
        ppi.parent = NULL;
        return -2;
    }

    int blockSize = vcb->blockSize;
    int dirBlocks = (thisDir[0].size + blockSize - 1) / blockSize;

    releaseBlocks(thisDir[0].blockLoc, dirBlocks);

    // Mark the directory entry as used
    ppi.parent[ppi.index].name[0] = '\0';
    saveDir(ppi.parent);

    free(thisDir);
    safeFree(ppi.parent); // NOT ROOT OR CWD
    ppi.parent = NULL;

    return 0;
}

dirEntry * findFreeDirEntry(dirEntry *de) {
    int numEntries = de[0].size / sizeof(dirEntry);

    for (int i = 0; i < numEntries; i++) {
        if (de[i].name[0] == '\0') {
            return &de[i];
        }
    }

    int newNumEntries = numEntries * CHANGE_DIR_FACTOR;
    // call function to expand directory
    if (changeDirSize(de, newNumEntries) != -1) {
        return &de[numEntries];
    }

    return NULL;
}

int changeDirSize(dirEntry *de, int newCountEntries) {
    // Check if directory size needs to change
    int curCountEntries = de[0].size / sizeof(dirEntry);
    if (curCountEntries == newCountEntries) {
        return -1;
    }

