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

    // Check if caller is asking for root stats
    if (ppi.index == -2) {
        buf->st_size = rootDir->size;
        buf->st_accesstime = rootDir->accessTime;
        buf->st_modtime = rootDir->modifyTime;
        buf->st_createtime = rootDir->createTime;

    // Otherwise get stats from index into parent
    } else {
        buf->st_size = ppi.parent[ppi.index].size;
        buf->st_accesstime = ppi.parent[ppi.index].accessTime;
        buf->st_modtime = ppi.parent[ppi.index].modifyTime;
        buf-> st_createtime = ppi.parent[ppi.index].createTime;

    }

