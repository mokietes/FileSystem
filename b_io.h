/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: b_io.h
*
* Description:: Interface of basic I/O Operations
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

typedef int b_io_fd;
void b_init();


b_io_fd b_open (char * filename, int flags);
b_io_fd b_getFCB();

int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

#endif

