/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: dirEntry.c
*
* Description:: Implements functionality for creating, 
* initializing, loading, and saving directories in the custom 
* file system.Creating new directories with createDir(), 
* including setting . and .. entries. Initializing the root 
* directory with initRootDir() and updating the global VCB.
* Loading and saving the root directory from/to disk using 
* loadRootDir() and saveRootDir().
**************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "dirEntry.h"
#include "fsLow.h"
#include "vcb.h"
#include "freeSpace.h"

#define MIN_ENTRIES 4   // minimum entries per directory

dirEntry *rootDir;
dirEntry *cwDir;

//Temp for writing directory to disk
void writeDir(dirEntry *dir, int blocksNeeded, int blockLoc) {
    LBAwrite(dir, blocksNeeded, blockLoc);
}

