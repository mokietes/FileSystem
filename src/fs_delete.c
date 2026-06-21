/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien
* Student IDs:: 923752034 , 923676946
* GitHub-Name:: thats-not-my-name , mokietes
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: fs_delete.c
*
* Description:: 
*
*
**************************************************************/

#include <string.h>
#include <stdio.h>

#include "mfs.h"
#include "parsePath.h"
#include "vcb.h"
#include "freeSpace.h"
#include "dirFunc.h"

int fs_delete(char* filename) {
    ppInfo ppi;
    int ppRet;

    char *pathCopy = strdup(filename);
    if (pathCopy == NULL) return -1;

    ppRet = parsePath(pathCopy, &ppi);

    // Check if path is valid
    if (ppRet != 0) return -1;

    // Check if last element of the path doesn't exist in the parent
    if (ppi.index == -1) return -1;

    // Check if it is the root
    if (ppi.index == -2) return -1; 

    // Check if last element of the path is a file
    if (fs_isFile(pathCopy) != 1) {
        safeFree(ppi.parent);
        ppi.parent = NULL;
        return -1;
    }

    int blockSize = vcb->blockSize;
    int fileLoc = ppi.parent[ppi.index].blockLoc;

    int fileBlocks = (ppi.parent[ppi.index].size + blockSize - 1) / blockSize;
    //printf("fs_delete file blocks: %d\n", fileBlocks);

    if (fileBlocks == 0) fileBlocks = 1;   // a freshly-created empty file still owns 1 block

    releaseBlocks(fileLoc, fileBlocks);

    // Mark the directory entry as unused
    ppi.parent[ppi.index].name[0] = '\0';
    saveDir(ppi.parent);

    safeFree(ppi.parent);
    ppi.parent = NULL;

    return 0;
}
