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

// set bit at position to 1, uses bitwise or with mask
int setBit(int bitmapIndex)
{
    if (freeSpaceMap == NULL) {
        return -1;
    }
    
    int byteIndex = bitmapIndex / BYTE_BITS;
    int bitOffset = bitmapIndex % BYTE_BITS;
    unsigned char mask = 1 << bitOffset;
    
    freeSpaceMap[byteIndex] |= mask;
    return 0;

}

