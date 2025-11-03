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
