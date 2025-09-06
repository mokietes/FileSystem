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


// int allocBlocks(int numBlocks); // Returns LBA location (e.g., 100)
// void writeDir(dirEntry *dir, int blocksNeeded, int blockLoc); // Wraps LBAwrite

dirEntry * createDir(int countEntries, dirEntry *parent) {
    //if (countEntries < MIN_ENTRIES) countEntries = MIN_ENTRIES;
    int blockSize = vcb->blockSize;

    int memNeeded = countEntries * sizeof(dirEntry);
    int blocksNeeded = (memNeeded + blockSize - 1) / blockSize;
    int memActual = blocksNeeded * blockSize;

    dirEntry *newDir = (dirEntry *) malloc(memActual); // Zero-initialized

    int actualEntries = memActual / sizeof(dirEntry);

    // Initialize empty entries
    for (int i = 2; i < actualEntries; i++) {
        newDir[i].name[0] = '\0';
        newDir[i].blockLoc = -1;
        newDir[i].isDir = 0;
        newDir[i].size = 0;
        newDir[i].createTime = 0;
        newDir[i].modifyTime = 0;
        newDir[i].accessTime = 0;
    }

    time_t t = time(NULL);
    int loc = allocBlocks(blocksNeeded); // LBA block where this dir starts

    // Self-entry `.`
    strcpy(newDir[0].name, ".");
    newDir[0].size = sizeof(dirEntry) * actualEntries;
    newDir[0].blockLoc = loc;
    newDir[0].createTime = t;
    newDir[0].modifyTime = t;
    newDir[0].accessTime = t;
    newDir[0].isDir = 1;

    // Parent-entry `..`
    strcpy(newDir[1].name, "..");
    if (parent == NULL) {
        // Root directory: parent is self
        newDir[1].blockLoc = loc;
        newDir[1].size = newDir[0].size;
        newDir[1].isDir = 1;
        newDir[1].createTime = t;
        newDir[1].modifyTime = t;
        newDir[1].accessTime = t;
    } else {
        newDir[1] = parent[0]; // Copy parent self-entry
        strcpy(newDir[1].name, "..");
    }

    // Write directory to disk
    writeDir(newDir, blocksNeeded, loc);

    return newDir; // might just return loc in your design
}


void initRootDir() {
    int rootEntries = DEFAULT_ENTRIES * ROOT_ENTRIES_MULT;
    rootDir = createDir(rootEntries, NULL);
    
    if (rootDir == NULL) {
        printf("Failed to create root directory\n");
        return;
    }

    cwDir = rootDir;

    // updates VCB with root directory information
    // calculates the size of root directory in blocks
    int blockSize = vcb->blockSize;
    int rootSize = (rootDir->size + blockSize - 1) / blockSize;
    vcb->rootLocation = rootDir->blockLoc;
    vcb->rootSize = rootSize;
    
    printf("Root directory created successfully at block %d with %d entries\n", 
        vcb->rootLocation, rootEntries);
}

void loadRootDir() {
    // loads root directory from disk
    if (vcb == NULL) {
        printf("VCB not initialized, cannot load root directory\n");
        return;
    }
    
    // calculates how many directory entries fit in the root directory blocks
    int rootBytes = vcb->rootSize * vcb->blockSize;
    int rootEntries = rootBytes / sizeof(dirEntry);
    
    // allocates memory for root directory
    rootDir = malloc(rootBytes);
    if (rootDir == NULL) {
        printf("Failed to allocate memory for root directory\n");
        return;
    }

    cwDir = rootDir;
    
void saveRootDir() {
    // saves root directory to disk
    if (rootDir == NULL || vcb == NULL) {
        printf("Root directory or VCB not initialized, cannot save\n");
        return;
    }
    
    // writes root directory to disk
    if (LBAwrite(rootDir, vcb->rootSize, vcb->rootLocation) != vcb->rootSize) {
        printf("Failed to write root directory to disk\n");
        return;
    }
    
    // printf("Root directory saved successfully to block %d\n", vcb->rootLocation);
}

void safeFree(dirEntry *de) {
    if (de == NULL || de == rootDir || de == cwDir) {
        return;
    }

    free(de);
}
