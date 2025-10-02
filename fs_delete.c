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

    // temporary fix for removing empty files of size 0
    // because files are initially allocated with 1 block in b_open w/ create flag

    if (ppi.parent[ppi.index].size == 0) {
        ppi.parent[ppi.index].size = 1;
        //printf("fs_delete file size: %d\n", ppi.parent[ppi.index].size);
    }

