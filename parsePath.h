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

