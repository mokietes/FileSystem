/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: fs_stat.c
*
* Description:: This file implements the function fs_stat.
*
*
**************************************************************/

#include <string.h>


#include "mfs.h"
#include "parsePath.h"
#include "vcb.h"

int fs_stat(const char *path, struct fs_stat *buf) {
    ppInfo ppi;
    int ppRet;

    char *pathCopy = strdup(path);
    if (pathCopy == NULL) return -1;

    ppRet = parsePath(pathCopy, &ppi);

    // Check if path is valid
    if (ppRet != 0) return -1;

    // Check if last element of the path doesn't exist in the parent
    if (ppi.index == -1) return -1;
