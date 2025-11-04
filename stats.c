/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Geetarth Meduri
* Student IDs:: 920455884
* GitHub-Name:: Geetarty
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: b_io.c
*
* Description:: 
* fs_stat is a file system function designed to retrieve metadata about a specified file or 
* directory. Similar to the stat() system call in Unix, it populates a struct fs_stat with 
* details such as the item's total size in bytes, the file system's I/O block size, the number 
* of allocated blocks, and timestamps for creation, last access, and last modification. It 
* acts as a primary interface for programs to query file and directory properties without 
* needing to delve into the underlying file system's internal structures.
*
**************************************************************/

#include <stdlib.h> // For malloc, free
#include <string.h> // For memcpy, strcmp
#include <stdio.h>  // For printf, fprintf (for debugging/errors)
#include <sys/types.h> // For off_t, blksize_t, blkcnt_t, time_t

// Include your file system's header files
#include "mfs.h"     // For fs_stat struct and function prototype
#include "vcb.h"     // For VCB structure and global vcb pointer
#include "dirEntry.h" // For dirEntry structure and rootDir, cwDir
#include "parsePath.h" // For parsePath function and ppInfo struct
#include "fsLow.h"   // For LBAread (used by loadDir)

// --- External Global Variables ---
// These are assumed to be defined in fsInit.c or other core files
// and declared extern in their respective headers (vcb.h, dirEntry.h)
extern VCB *vcb;
extern dirEntry *rootDir;
extern dirEntry *cwDir;

// --- Helper Functions (from parsePath.c, or re-declared if needed) ---
// Note: If parsePath.h includes prototypes for loadDir, findInDir, isDirEntryDir,
// you don't need to re-declare them here. Just ensure parsePath.h is included.
// If loadDir is not in parsePath.h, you might need to add its prototype here or in parsePath.h.

// --- fs_stat Implementation ---
int fs_stat(const char *pathname, struct fs_stat *buf) {
    if (buf == NULL) {
        // Handle error: buffer pointer is NULL
        fprintf(stderr, "fs_stat error: Output buffer is NULL.\n");
        return -1;
    }

    // Allocate ppInfo structure on the stack
    ppInfo ppi;

    // Use parsePath to resolve the pathname
    // Make a mutable copy of pathname if parsePath modifies it (strtok does)
    char *pathCopy = strdup(pathname);
    if (pathCopy == NULL) {
        fprintf(stderr, "fs_stat error: Failed to duplicate pathname.\n");
        return -1;
    }

    int parseResult = parsePath(pathCopy, &ppi);
    free(pathCopy); // Free the duplicated path as soon as it's parsed

    if (parseResult == -1) {
        // Error during path parsing (e.g., path is too long, invalid characters, LBAread failures
        // in loadDir)
        fprintf(stderr, "fs_stat error: Invalid path or parsing failed for '%s'.\n", pathname);
        return -1;
    }

    dirEntry *targetDirContent = NULL; // Pointer to the loaded directory content
    dirEntry *targetEntry = NULL;      // Pointer to the specific dirEntry for the target file/dir

    if (ppi.index == -2) { // Path refers to the root directory itself ("/")
        targetEntry = rootDir;
        // rootDir is usually a global variable and its content doesn't need to be freed by 
        // fs_stat
    } else if (ppi.index != -1) { // Path to an existing file/directory
        // Load the parent directory's content to access the target dirEntry
        // Remember that loadDir allocates memory, which MUST be freed later.
        targetDirContent = loadDir(ppi.parent);
        if (targetDirContent == NULL) {
            fprintf(stderr,"fs_stat error: Could not load parent directory for '%s'.\n", pathname);
            return -1;
        }
        targetEntry = &(targetDirContent[ppi.index]);
    } else { // ppi.index == -1: The last element of the path does not exist
        fprintf(stderr, "fs_stat error: File or directory '%s' not found.\n", pathname);
        return -1;
    }

    // Populate the fs_stat structure with information from targetEntry and VCB
    buf->st_size = targetEntry->size;
    buf->st_accesstime = targetEntry->accessTime;
    buf->st_modtime = targetEntry->modifyTime;
    buf->st_createtime = targetEntry->createTime;

    // Check if VCB is initialized (it should be by fsInit)
    if (vcb == NULL) {
        fprintf(stderr, "fs_stat error: File system VCB is not initialized.\n");
        // Free dynamically allocated memory before returning
        if (targetDirContent != NULL) {
            free(targetDirContent);
        }
        return -1;
    }
    buf->st_blksize = vcb->blockSize;

    // Calculate st_blocks, handling potential division by zero if blockSize is invalid
    if (buf->st_blksize > 0) {
        buf->st_blocks = (buf->st_size + buf->st_blksize - 1) / buf->st_blksize;
    } else {
        fprintf(stderr, "fs_stat error: Invalid block size (zero or negative) in VCB.\n");
        // Free dynamically allocated memory before returning
        if (targetDirContent != NULL) {
            free(targetDirContent);
        }
        buf->st_blocks = 0; // Or some other error indicator
        return -1;
    }

    // --- IMPORTANT: Memory Cleanup ---
    // If loadDir allocated memory for targetDirContent (which it does), it must be freed here.
    // Do this only if targetDirContent was actually allocated by loadDir.
    if (targetDirContent != NULL) {
        free(targetDirContent);
        targetDirContent = NULL; // Good practice to set to NULL after freeing
    }

    return 0; // Success
}

