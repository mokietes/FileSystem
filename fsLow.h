/**************************************************************
* Class::  CSC-415
* Name:: Professor Bierman
* Student IDs:: N/A
* GitHub-Name:: bierman
* Group-Name:: N/A
* Project:: Basic File System
*
* File:: fsLow.h
*
* Description:: This file provides the ability to read and write 
*   Logical Blocks and is the main interface for the file system
*   project.  
*   Note that the layer uses one more block than that presented
*   to the file organization module.  This block is used to 
*   hold the partition information and is not accessible from
*   any other layer.  But, when you use the hexdump utility
*   you will see that the first block is not part of the volume.
*
*   The file created by this layer represents the physical hard 
*	drive.  It presents to the logical layer (your layer) as just
*	a logical block array (a series of blocks - nominally 512 bytes,
*	that the logical layer can utilize). 
*
*	It is imperative that the logical layer (your layer) first
*	call startPartitionSystem before using any function and when
*	finished calls closePartitionSystem() to ensure that the 
*	file that represents the physical drive is properally closed.
*
**************************************************************/
//
// Start Partition System
//
// This is the first function to call before your filesystem starts
// If the filename already exists, then the input values stored in 
// volSize and blockSize are ignored.  If the file does not exist, it will 
// be created to the specified volume size in units of the block size
// (must be power of 2) plus one for the partition header.
//
// On return 
// 		return value 0 = success;
//		return value -1 = file exists but can not open for write
//		return value -2 = insufficient space for the volume		
//		volSize will be filled with the volume size
//		blockSize will be filled with the block size
#ifndef uint64_t
typedef u_int64_t uint64_t;
