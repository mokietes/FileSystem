/****************************************************************************
 *	hexdump - a program to display a file in hexadecimal and ascii
 *	Such as:
 *		000020: 19 00 00 00 48 00 00 00  5F 5F 50 41 47 45 5A 45 | ....H...__PAGEZE
 *
 *  Author:		Robert Bierman
 *	Date:		March 28, 2020
 *	Source:		hexdump.c
 *
 *  Primary Purpose:
 *	Written as a utility for CSC-415 Operating Systems, File System project
 *  to allow the dumping of the "drive" file for verification of proper 
 *  structure and content.
 *
 *  copyright 2020 Robert Bierman
 ****************************************************************************/

// Compilation:	gcc hexdump.c -o hexdump

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif


#define BUFSIZE 4096		//amount read at one time
#define BLOCKSIZE 256		// number of bytes printed before a blank line
#define LBABLOCKSIZE	512	// display blocks from command line are based on 512 bytes
#define VERSION "1.0"		// Version

// This procedure takes a file name, a starting block and a number of blocks and dumps the
// file to stdout.  
// Output (including the number of blocks) is limited by the length of the file and
// partial blocks are counted as a whole block for the header showing how many blocks are
// being displayed.
//
// Checks are done to ensure that blanks are displayed on the last line of the output
// if the file only partially uses the last 16 bytes.

int processFile (char * filename, uint64_t startBlock, uint64_t numBlocks)
	{
	int readbytes;
	int position = 0;
	int loops = BUFSIZE / BLOCKSIZE;	//number of loops of blocks within one buffer read
	int offset;
	int k;
	uint32_t lbaBlockSize = LBABLOCKSIZE;
	uint64_t numBytesToStartBlock;
	uint64_t numBytesToProcess;
	uint64_t endOfFile;

	numBytesToProcess = numBlocks * lbaBlockSize;
	numBytesToStartBlock = startBlock * lbaBlockSize;
	
	int fd = open (filename, O_RDONLY);			//open the file
	
	// Error opening file (common if they don't enter a valid file name)
	if (fd == -1)
		{
		printf ("ERROR: failed to open file '%s'\n", filename);
		return -2;
		}
		
	endOfFile = lseek(fd, 0, SEEK_END);  //will reset seek below
	
	if (numBytesToProcess == 0)
		{
		numBytesToProcess = endOfFile;
		//reset numBlocks for the header here
		numBlocks = ((numBytesToProcess + lbaBlockSize) - 1) / lbaBlockSize;
		numBlocks = numBlocks - startBlock;
		}

	unsigned char * buf = malloc (BUFSIZE);		//Allocate the read buffer
	
	// Very rare error - something bad if I can not allocate a small buffer
	if (buf == NULL)
		{
		close (fd);
		printf ("Failed to allocate buffer\n");
		return -3;
		}
	
	//Position to the startBlock
	lseek (fd, numBytesToStartBlock, SEEK_SET);
	position = numBytesToStartBlock;
	
	if (position > endOfFile)		//can not start past the end of the filename
		{
		printf ("Can not dump file %s, starting at block %llu, past the end of the file.\n\n", 
			filename, (unsigned long long)startBlock);	
		return (-5);
		}

	// calculate max blocks we can display from the given start point
	uint64_t maxBlocks = (((endOfFile - position) + lbaBlockSize) - 1) / lbaBlockSize;
	if (numBlocks > maxBlocks)
		numBlocks = maxBlocks;

	
	//Proces the file - the do loop goes until we read less bytes than the BUFSIZE
	printf ("Dumping file %s, starting at block %llu for %llu block%c:\n\n", 
		filename, (unsigned long long)startBlock, (unsigned long long)numBlocks, numBlocks != 1?'s':'\0');	
	do
		{
		if (position >= (numBytesToStartBlock + numBytesToProcess))
			goto cleanup;
			
		readbytes = read (fd, buf, BUFSIZE);		//Read one block
		offset = 0;									//set our offset within the block
		for (int i = 0; i < loops; i++)				//Loop for each "Block" within one buffer read
			{
			for (int j = 0; j < BLOCKSIZE/16; j++)	//loop j lines for each block
				{
				if (position+offset >= (numBytesToStartBlock + numBytesToProcess))
					goto cleanup;
				
				// Handle if we are at the end of the file and the line will have less 
				// than 16 bytes associated with it.
				if (offset + 16 > readbytes)
					{
					printf ("%06X: ", offset+position);
					for (k = 0; k < readbytes - offset; k++)
						{
						printf ("%02X ", buf[offset + k]);
						}
					for (;k < 16; k++)	
						{
						printf ("   ");   //Print remaining of the hex output as blanks to fill out the line
