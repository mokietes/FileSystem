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

    while (1) 
    {

        // Find token1's index into its parent directory
        int idx = findInDir(parent, token1);
        token2 = strtok_r(NULL, "/", &savePtr);

        // Check if token1 is the last element of the pathname
        // The loop exits and the function terminates once the last element
        // is found, and the ppi struct is filled
        if (token2 == NULL) {
            ppi->parent = parent;
            ppi->index = idx;
            ppi->lastElement = token1;
            return 0;
        } else {
            // token1 is not the last element of the pathname

            // Check if token1 exists in the parent
            if (idx ==  -1) return -1;
        
            // Check if token1 is a directory
            if (!isDirEntryDir(&(parent[idx]))) return -1;


            if (strcmp(token1, "..") == 0) {
                if (parent == rootDir) {
                    token1 = token2;
                    continue;
                }

                dirEntry *tempDir = loadDir(&parent[idx]);
                if (tempDir == NULL) {
                    if (parent != startParent) free(parent);
                    return -1;
                }

                if (parent != startParent) free(parent);
                parent = tempDir;

            } else {
                dirEntry *tempDir = loadDir(&parent[idx]);
                if (tempDir == NULL) {
                    if (parent != startParent) free(parent);
                    return -1;
                }
                if (parent != startParent) free(parent);
                parent = tempDir;
            }


            /*

            // If token1 is a directory, load the directory from disk
            // and set it as the parent, and token2 as the new token1
            // This is for the next iteration to check if it is the last
            // element in the pathname.
            dirEntry *tempDir = loadDir(&parent[idx]);

            if (strcmp(token1, "..") == 0) {
                if (parent == rootDir) {
                    tempDir = rootDir;
                }
            }
        
            // Make sure to not free root or cwd
            if (tempDir != startParent) {
                free(parent);

                // Check if loadDirectory failed due to malloc/LBAread errors
                if (tempDir == NULL) {
                    parent = NULL;
                    return -1;
                }
            }

            parent = tempDir;

            */
            token1 = token2;
        }

    }
}

int findInDir(dirEntry *de, char *name) {
    // Find number of entries within the passed in directory de
    int numEntries = de[0].size / sizeof(dirEntry);

    // Handle special directory entries
    if (strcmp(name, ".") == 0) {
        return 0;
    }

    if (strcmp(name, "..") == 0) {
        return 1;
    }

    // Loop through de to find the matching directory entry name
    for (int idx = 0; idx < numEntries; idx++) {
        //printf("findInDir[%d]: %s\n", idx,  de[idx].name);
        if (strcmp(de[idx].name, name) == 0) {
            return idx;
        }
    }

    return -1;
}

int isDirEntryDir(dirEntry *de) {
    if (de->isDir == 1) {
        return 1;
    }

    return 0;
}

dirEntry * loadDir(dirEntry *de) {
    // Get the starting block and number of blocks for the directory entry
    int blockLoc = de->blockLoc;
    int blockSize = vcb->blockSize;
    int blocksNeeded = (de->size + blockSize - 1) / blockSize;

    char *buffer = malloc(blocksNeeded * blockSize);
    if (buffer == NULL) {
        return NULL;
    }

