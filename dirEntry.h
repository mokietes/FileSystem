/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: dirEntry.h
*
* Description:: This header contains the directory entry struct
* and function prototypes for creating directories, and for
* managing the root directory. 
*
**************************************************************/

#ifndef DIR_ENTRY_H
#define DIR_ENTRY_H
#include <time.h>

#define MAX_NAME 15
#define DEFAULT_ENTRIES 50
#define ROOT_ENTRIES_MULT 4

typedef struct dirEntry {
    char name[MAX_NAME + 1]; // filename or dir name
    time_t createTime;
    time_t modifyTime;
    time_t accessTime;
    int size;                // file size in bytes (0 for directories)
    int blockLoc;            // starting block
    char isDir;              // 1 = dir, 0 = file
} dirEntry;

