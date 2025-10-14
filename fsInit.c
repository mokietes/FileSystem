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

