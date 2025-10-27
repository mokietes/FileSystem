/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien
* Student IDs:: 923752034 , 923676946
* GitHub-Name:: thats-not-my-name , mokietes
* Group-Name:: Epsilon
* Project:: Epsilon File System
*
* File:: vcb.h
*
* Description:: This header file contains the VCB struct 
* definition and VCB function prototypes.
*
**************************************************************/

#ifndef VCB_H
#define VCB_H

typedef struct VCB {
    long signature;         // identifier for the VCB
    int totalBlocks;        // number of blocks in volume
    int blockSize;          // size of a block
    int bitmapStart;        // tracks where bitmap begins
    int bitmapBlocks;       // blocks the bitmap occupies
    int rootLocation;       // location of the root directory
    int rootSize;           // size of root directory in block numbers
    int firstBlockLocation; // location of the first block
    int totalFreeSpace;     // number of free blocks
} VCB;

