/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: dirEntry.h
*
* Description:: This header contains the directory entry struct
* and function prototypes for creating directories, and for
* managing the root directory. 
*
**************************************************************/

#ifndef DIR_ENTRY_H
#define DIR_ENTRY_H
#include <time.h>

#define MAX_NAME 15
#define DEFAULT_ENTRIES 50
#define ROOT_ENTRIES_MULT 4
