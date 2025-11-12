/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien
* Student IDs:: 923752034 , 923676946
* GitHub-Name:: thats-not-my-name , mokietes
* Group-Name:: Epsilon
* Project:: Epsilon File System
*
* File:: vcb.c
*
* Description:: This file contains the functions for
* initializing the VCB and writing the VCB to the volume.
*
**************************************************************/

#include <stdio.h>
#include <sys/types.h>

#include "vcb.h"
#include "fsLow.h"
#include "freeSpace.h"
#include "dirEntry.h"

void initVCB(int numberOfBlocks, int blockSize, long signature) {
    vcb->totalBlocks = numberOfBlocks;
    vcb->blockSize = blockSize;
    vcb->signature = signature;

    // initializes the free space management
    if (initFreeSpace(numberOfBlocks, blockSize) != 0) {
        printf("Failed to initialize free space management\n");
        return;
    }

