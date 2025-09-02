/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: bitmap.c
*
* Description:: Bitmap manipulation functions for filesystem 
* free space management.
*
**************************************************************/

#include <stdlib.h>

#include "bitmap.h"
#include "freeSpace.h"

extern char *freeSpaceMap;
