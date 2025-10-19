/**************************************************************
* Class::  CSC-415-0# Spring 2024
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
* Project:: Basic File System
*
* File:: fsShell.c
*
* Description:: Main driver for file system assignment.
*
* Make sure to set the #defined on the CMDxxxx_ON from 0 to 1 
* when you are ready to test that feature
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"

#define PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define SINGLE_QUOTE	0x27
#define DOUBLE_QUOTE	0x22
#define BUFFERLEN		200
#define DIRMAX_LEN		4096
