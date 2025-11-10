/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: parsePath.h
*
* Description:: This header file contains the struct definition
* for the parse path information and the function prototypes
* for the functions needed to parse a pathname.
*
**************************************************************/

#ifndef PARSE_PATH_H
#define PARSE_PATH_H

#include "dirEntry.h"

typedef struct ppInfo {
    dirEntry *parent;  // parent directory of the pathname's dirEntry
    int index;         // index into parent, -1 if it doesnt exist, -2 if it is root
    char *lastElement; // name of the last element in the pathname, NULL if root
} ppInfo;

// returns 0 for valid, -1 for invalid
int parsePath(char *pathname, ppInfo *ppi);

// returns index of a directory entry name within the directory de
// returns -1 if not found
int findInDir(dirEntry *de, char *name);

// Checks if passed in directory entry is a directory
// returns 1 if it is a directory, 0 otherwise
int isDirEntryDir(dirEntry *de);

// loads the directory from disk
dirEntry * loadDir(dirEntry *de);

#endif
