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



//This part is the assignment original code. 

// b_io_fd b_open (char * filename, int flags)
// 	{
// 	b_io_fd returnFd;

// 	//*** TODO ***:  Modify to save or set any information needed
// 	//
// 	//
		
// 	if (startup == 0) b_init();  //Initialize our system
	
// 	returnFd = b_getFCB();				// get our own file descriptor
// 										// check for error - all used FCB's
	
// 	return (returnFd);						// all set
// 	}

b_io_fd b_open(char *filename, int flags)
{
    if (startup == 0) b_init();

    b_io_fd fd = b_getFCB();  // get free file control block
    if (fd < 0) return -1;

    ppInfo info;
    if (parsePath(filename, &info) != 0)
        return -1;

    dirEntry *target = NULL;

    if (info.index == -1) {
        if (!(flags & O_CREAT)) return -1;

        target = findFreeDirEntry(info.parent);
        if (!target) return -1;

        int block = allocBlocks(1);
        if (block == -1) return -1;

        time_t now = time(NULL);
        strncpy(target->name, info.lastElement, MAX_NAME);
        target->name[MAX_NAME] = '\0';
        target->isDir = 0;
        target->size = 0;
        target->blockLoc = block;
        target->createTime = now;
        target->modifyTime = now;
        target->accessTime = now;

        saveDir(info.parent);
    } else {
        target = &info.parent[info.index];

        time_t now = time(NULL);
        target->accessTime = now;
        target->modifyTime = now;
        if (target->isDir) return -1;
    }

    fcbArray[fd].buf = malloc(vcb->blockSize);
    fcbArray[fd].index = 0;
    fcbArray[fd].buflen = 0;
    fcbArray[fd].blockLoc = target->blockLoc;
    fcbArray[fd].fileSize = target->size;
    fcbArray[fd].flags = flags;
    fcbArray[fd].dirty = 0;
    fcbArray[fd].entry = target;

    fcbArray[fd].parent = info.parent;

    return fd;
}


// Interface to seek function	
int b_seek(b_io_fd fd, off_t offset, int whence)
{
	// Ensure system is initialized
    if (startup == 0) b_init();
    if (fd < 0 || fd >= MAXFCBS || fcbArray[fd].buf == NULL)
        return -1;

    b_fcb *fcb = &fcbArray[fd];

    switch (whence) {
		// Set pointer to offset
        case SEEK_SET: fcb->index = offset; break;    

		// Move pointer forward/backward
        case SEEK_CUR: fcb->index += offset; break;

		// Offset from end
        case SEEK_END: fcb->index = fcb->fileSize + offset; break;
        default: return -1;
    }

    return fcb->index;
}



// Interface to write function	
int b_write(b_io_fd fd, char *buffer, int count)
{
    if (startup == 0) b_init();
    if (fd < 0 || fd >= MAXFCBS || fcbArray[fd].buf == NULL)
        return -1;

    b_fcb *fcb = &fcbArray[fd];
    int totalWritten = 0;

    while (count > 0) {
        int offset = fcb->index % vcb->blockSize;
        int space = vcb->blockSize - offset;
        int toWrite = (count < space) ? count : space;

        if (offset == 0 && fcb->dirty)
            LBAwrite(fcb->buf, 1, fcb->blockLoc);

        memcpy(fcb->buf + offset, buffer, toWrite);
        fcb->dirty = 1;

        fcb->index += toWrite;
        if (fcb->index > fcb->fileSize)
            fcb->fileSize = fcb->index;

        buffer += toWrite;
        count -= toWrite;
        totalWritten += toWrite;
    }

    return totalWritten;
}




// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill 
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
// int b_read (b_io_fd fd, char * buffer, int count)
// 	{

// 	if (startup == 0) b_init();  //Initialize our system

// 	// check that fd is between 0 and (MAXFCBS-1)
// 	if ((fd < 0) || (fd >= MAXFCBS))
// 		{
// 		return (-1); 					//invalid file descriptor
// 		}
		
// 	return (0);	//Change this
// 	}

int b_read(b_io_fd fd, char *buffer, int count)
{
	//initiate system
    if (startup == 0) b_init();

	// Invalid file descriptor
    if (fd < 0 || fd >= MAXFCBS || fcbArray[fd].buf == NULL)
        return -1;

    b_fcb *fcb = &fcbArray[fd];   // Get file control block
    int totalRead = 0;

    while (count > 0 && fcb->index < fcb->fileSize) {
        int offset = fcb->index % vcb->blockSize;		// Offset inside the block
        int space = vcb->blockSize - offset;			// Space left in block
        int toRead = (count < space) ? count : space;	// Read only what fits

		// Load block from disk
        LBAread(fcb->buf, 1, fcb->blockLoc);

		// Copy to user's buffer
        memcpy(buffer, fcb->buf + offset, toRead);

		// Move file pointer
        fcb->index += toRead;
        buffer += toRead;
        count -= toRead;
        totalRead += toRead;
    }

    return totalRead;
}

	
// Interface to Close the file	
int b_close(b_io_fd fd)
{
	// Validate descriptor
    if (fd < 0 || fd >= MAXFCBS) return -1;
    b_fcb *fcb = &fcbArray[fd];

	// If buffer modified write to disk
    if (fcb->dirty)
        LBAwrite(fcb->buf, 1, fcb->blockLoc);

	// Save updated file size and modify time
    fcb->entry->size = fcb->fileSize;
    fcb->entry->modifyTime = time(NULL);

	//Save parent's directory changes
    saveDir(fcb->parent);
    
	//Free buffer as well as reset VCB
    free(fcb->buf);
    memset(fcb, 0, sizeof(b_fcb));

    return 0;
