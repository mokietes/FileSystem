/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: fsInit.c
*
* Description:: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "freeSpace.h"
#include "dirEntry.h"

#define SIGNATURE 1123581321345589144

VCB *vcb = NULL;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* ADD any code you need to initialize your file system. */

	// allocates a block of memory for global VCB pointer
	vcb = malloc(sizeof(VCB));
	if (vcb == NULL) {
		return 1;
	}

	// LBAread blocks 0 into VCB pointer through a buffer
	char *buffer = malloc(blockSize);
	if (LBAread(buffer, 1, 0) != 1) {
		free(vcb);
		vcb = NULL;
		return 1;
	}

	memcpy(vcb, buffer, sizeof(VCB));
	free(buffer);
	buffer = NULL;

	// determines using the signature if the volume needs to be formatted or not
	if (vcb->signature != SIGNATURE) {
		printf("Formatting new filesystem...\n");
		initVCB(numberOfBlocks, blockSize, SIGNATURE);
		writeVCB();
		printf("Filesystem formatting complete.\n");
	} else {
		printf("Loading existing filesystem...\n");
		// loads free space map from disk
		if (loadFreeSpace() == -1) {
			return 1;
		}
		// loads root directory from disk
		loadRootDir();
	}

	return 0;
	}
