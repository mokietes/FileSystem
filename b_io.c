/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: b_io.c
*
* Description:: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#include "vcb.h"
#include "freeSpace.h"
#include "dirEntry.h"
#include "parsePath.h"
#include "fsLow.h"  
#include "dirFunc.h"



#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	/** TODO add al the information you need in the file control block **/
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int blockLoc;
    int fileSize;
    int flags;
    int dirty;
    dirEntry *entry;
    dirEntry *parent;
	} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
		//init fcbArray to all free
		for (int i = 0; i < MAXFCBS; i++)
			{
			fcbArray[i].buf = NULL; //indicates a free fcbArray
			}
			
		startup = 1;
		// Load root directory and set current working directory
        

        // NOTE: rootDir is already loaded, cwd is handled elsewhere
        // - Derek

        /* 
		rootDir = loadDir(&((dirEntry)
		{
			.blockLoc = vcb->rootLocation,
			.size = vcb->rootSize * vcb->blockSize,
			.isDir = 1
		}));
		cwDir = rootDir;
		if (cwDir == NULL) {
			printf("ERROR: b_init failed to load rootDir\n");
		}
        */

	}

//Method to get a free FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].buf == NULL)
			{
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR


