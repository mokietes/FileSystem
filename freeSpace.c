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

    // Write initial bitmap to disk
    LBAwrite(freeSpaceMap, bitmapBlocks, BITMAP_START);

    vcb->bitmapStart = BITMAP_START;
    vcb->bitmapBlocks = bitmapBlocks;
    vcb->firstBlockLocation = BITMAP_START + bitmapBlocks;
    vcb->totalFreeSpace = numberOfBlocks - bitmapBlocks - 1;
    return 0;
}

int loadFreeSpace() {
    if (vcb == NULL) {
        printf("VCB not initialized, cannot load free space map\n");
        return -1;
    }

    int bitmapStart = vcb->bitmapStart;
    int bitmapBlocks = vcb->bitmapBlocks;
    int blockSize = vcb->blockSize;

    freeSpaceMap = malloc(bitmapBlocks * blockSize);
    if (freeSpaceMap == NULL) {
        return -1;
    }

    if (LBAread(freeSpaceMap, bitmapBlocks, bitmapStart) != bitmapBlocks) {
        free(freeSpaceMap);
        freeSpaceMap = NULL;
        return -1;
    }

    printf("Free space map loaded successfully from block %d\n", bitmapStart);

    return 0;
}

int allocBlocks(int numBlocks) {
    if (numBlocks > vcb->totalFreeSpace) {
        return -1;
    }

    // counters to find contiguous number of blocks equal to numBlocks
    int contigBlocks = 0;
    int startBlock = -1;

    int firstBlock = vcb->firstBlockLocation;
    int totalBlocks = vcb->totalBlocks;

    for (int i = firstBlock; i < totalBlocks; i++) {
        int byteIndex = i / BYTE_BITS;
        int bitOffset = i % BYTE_BITS;
        
        // checks if the bit representing the current block is unused (0)
        int currentBit = (freeSpaceMap[byteIndex] >> bitOffset) & 1;

        if (currentBit == 0) {
            if (contigBlocks == 0) {
                startBlock = i;
            }
            contigBlocks++;

            if (contigBlocks == numBlocks) {
                // marks all allocated blocks as used
                for (int j = startBlock; j < startBlock + numBlocks; j++) {
                    setBit(j);
                }

