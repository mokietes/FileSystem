/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough,
* Geetarth Meduri
* Student IDs:: 923752034 , 923676946 , 921749371, 920455884
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03,
* Geetarty
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: freeSpace.c
*
* Description:: This file contains the functions for
* initializing the free space bitmap and allocating blocks
* from the total free space.
*
**************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "freeSpace.h"
#include "fsLow.h"
#include "bitmap.h"
#include "vcb.h"

char *freeSpaceMap;

int initFreeSpace(int numberOfBlocks, int blockSize) {
    // Using a bitmap for free space management
    // So each block in the volume is represented with 1 bit in the bitmap
    int bitmapBytes = (numberOfBlocks + BYTE_BITS - 1) / BYTE_BITS;
    int bitmapBlocks = (bitmapBytes + blockSize - 1) / blockSize;

    freeSpaceMap = malloc(bitmapBlocks * blockSize);
    if (freeSpaceMap == NULL) {
        return -1;
    }

    // Marking the VCB as used
    if (setBit(VCB_START) == -1) {
        return -1;
    }

    // Marking the bitmap's own occupied blocks as used
    for (int i = 0; i < bitmapBlocks; i++) {
        if (setBit(BITMAP_START + i) == -1) {
            return -1;
        }
    }

