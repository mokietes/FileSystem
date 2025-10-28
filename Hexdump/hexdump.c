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

