/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: parsePath.c
*
* Description:: This file contains the functions needed to
* parse a pathname string to check its validity and whether it
* exits, and returns information about its parent directory,
* the index into its parent directory or whether it is the root,
* and the name of the last element in the pathname.
*
**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "parsePath.h"
#include "vcb.h"
#include "fsLow.h"

