/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: parsePath.c
*
* Description:: This file contains the functions needed to
* parse a pathname string to check its validity and whether it
* exits, and returns information about its parent directory,
* the index into its parent directory or whether it is the root,
* and the name of the last element in the pathname.
*
**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "parsePath.h"
#include "vcb.h"
#include "fsLow.h"

int parsePath(char *pathname, ppInfo *ppi) {
    dirEntry *startParent; // The starting parent directory, either root or cwd
    dirEntry *parent;
    char *savePtr;
    char *token1;
    char *token2;

    // Handle if passed in pathname is NULL
    if (pathname == NULL) return -1;

    char pathCopy[strlen(pathname) + 1];
    strcpy(pathCopy, pathname);

    // If pathname is an absolute path, set starting parent to the root
    if (pathCopy[0] == '/') {
        startParent = rootDir;
    } else {
        // If pathname is relative, set starting parent to current working directory
        startParent = cwDir;
    }

    parent = startParent;
    
    token1 = strtok_r(pathname, "/", &savePtr);

    if (token1 == NULL) {
        // Handle special case if the pathname is just the root directory
        if (pathname[0] ==  '/') {
            ppi->parent = parent;
            ppi->index = -2; // Used to indicate that it is the root itself
            ppi->lastElement = NULL; // No last element for root
            return 0;
        } else {
            // Otherwise the pathname is invalid
            return -1;
        }
    }

