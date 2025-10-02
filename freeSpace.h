/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: freeSpace.h
*
* Description:: This header file contains the constants and
* function prototypes for free space management.
*
**************************************************************/

#ifndef FREE_SPACE_H
#define FREE_SPACE_H

#define BYTE_BITS 8
#define VCB_START 0
#define BITMAP_START 1

extern char *freeSpaceMap;

int initFreeSpace(int numberOfBlocks, int blockSize);
int loadFreeSpace();
int allocBlocks(int numBlocks);
int releaseBlocks(int blockLoc, int numbBlocks);

#endif
