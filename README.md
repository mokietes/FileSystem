# CSC415 Group Term Assignment - File System

This is a GROUP assignment written in C.  Only one person on the team needs to submit the project.

**Assignment Purpose and Learning Outcomes:**
- Large scale project planning
- Working in groups
- Understanding the file system
- Understanding of low level file functionality
- Multi-Phased Project (multiple due dates)
- Advanced buffering
- Freespace allocation and release management
- Persistence
- Directory structures
- Tracking file information 

**Let me remind you of the general expectations for all projects:** 
- All projects are done in C in the Ubuntu Linux Virtual Machine.
- Code must be neat, with proper and consistent indentation and well documented. 
- Keep line to around 80 characters per line, and not line greater than 100 characters.
- Comments must describe the logic and reason for the code and not repeat the code.  
- Variables must have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.
- You must use `make` to compile the program.
- You must use `make run` (sometimes with RUNOPTIONS) to execute the program.
- In addition, each file must have the standard header as defined below.
- All source files and writeup must be in the main branch of the github.

Each .c and .h file must have a standard header as defined below.  Make sure to put in your section number (replace the #), your name, your student IDs, a proper project name, GitHub name, Group name, filename, and description of the project.  Do not alter the number of asterisks and ensure that the header starts on line 1 of the file.

```
/**************************************************************
* Class::  CSC-415-0# Spring 2025
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
* Project:: Basic File System
*
* File:: <name of this file>
*
* Description::
*
**************************************************************/
```



Your team have been designing components of a file system.  You have defined the goals and designed the directory entry structure, the volume structure and the free space.  Now it is time to implement your file system.

While each of you can have your own github, only one is what you use for the project to be turned in.  Make sure to list that one on the writeups.

The project is in three phases.  The first phase is the "formatting" the volume.  This is further described in the steps for phase one and the phase one assignment.

The second phase is the implementation of the directory based functions.  See Phase two assignment.

The final phase is the implementation of the file operations.

To help I have written the low level LBA based read and write.  The routines are in fsLow.o, the necessary header for you to include file is fsLow.h.  You do NOT need to understand the code in fsLow, but you do need to understand the header file and the functions.  There are 2 key functions:



`uint64_t LBAwrite (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);`

`uint64_t LBAread (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);`

LBAread and LBAwrite take a buffer, a count of LBA blocks and the starting LBA block number (0 based).  The buffer must be large enough for the number of blocks * the block size.

On return, these function returns the number of **blocks** read or written.



In addition, I have written a hexdump utility that will allow you to analyze your volume file in the Hexdump subdirectory.

**Your assignment is to write a file system!** 

You will need to format your volume, create and maintain a free space management system, initialize a root directory and maintain directory information, create, read, write, and delete files, and display info.  See below for specifics.

I have provided an initial “main” (fsShell.c) that will be the driver to test you file system.  Your group can modifiy this driver as needed.   The driver will be interactive (with all built in commands) to list directories, create directories, add and remove files, copy files, move files, and two “special commands” one to copy from the normal filesystem to your filesystem and the other from your filesystem to the normal filesystem.

You should modify this driver as needed for your filesystem, adding the display/setting of any additional meta data, and other functions you want to add.

The shell also calls two function in the file fsInit.c `initFileSystem` and `exitFileSystem` which are routines for you to fill in with whatever initialization and exit code you need for your file system.  

Some specifics - you need to provide the following interfaces:

```
b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

```

Note that the function are similar to the b_read you have done, there is a signifigant difference since you now write the function to find the file information.  
You have to have methods of locating files, and knowing which logical block addresses are associated with the file.

The Open command must support the following flags:
```
O_CREAT   - Creates a new file is it does not exist (ignored if the file does exist)
O_TRUNC   - Set the file length to 0 (truncates all data)
O_APPEND  - Sets the file position to the end of the file (Same as doing a seek 0 from SEEK_END)
O_RDONLY  - File can only do read/seek operations
O_WRONLY  - File can only do write/seek operations
O_RDWR    - File can be read or written to
```

Directory Functions - see [https://www.thegeekstuff.com/2012/06/c-directory/](https://www.thegeekstuff.com/2012/06/c-directory/) for reference.

```
int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

char * fs_getcwd(char *pathbuffer, size_t size);
int fs_setcwd(char * pathname);   //linux chdir
int fs_isFile(char * filename);	//return 1 if file, 0 otherwise
int fs_isDir(char * pathname);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file

// This is NOT the directory entry, it is JUST for readdir.
struct fs_diriteminfo
    {
    unsigned short d_reclen;    /* length of this record */
    unsigned char fileType;    
    char d_name[256]; 			/* filename max filename is 255 characters */
    };
```
Finally file stats - not all the fields in the structure are needed for this assingment

```
int fs_stat(const char *filename, struct fs_stat *buf);

struct fs_stat
    {
    off_t     st_size;    		/* total size, in bytes */
    blksize_t st_blksize; 		/* blocksize for file system I/O */
    blkcnt_t  st_blocks;  		/* number of 512B blocks allocated */
    time_t    st_accesstime;   	/* time of last access */
    time_t    st_modtime;   	/* time of last modification */
    time_t    st_createtime;   	/* time of last status change */
	
    * add additional attributes here for your file system */
    };

```

These interfaces will also be provided to you in mfs.h.

**Note:** You will need to modify mfs.h for the fdDIR strucutre to be what your file system need to maintain and track interation through the directory structure.

A shell program designed to demonstrate your file system called fsshell.c is proviced.  It has a number of built in functions that will work if you implement the above interfaces, these are:
```
ls - Lists the file in a directory
cp - Copies a file - source [dest]
mv - Moves a file - source dest
md - Make a new directory
rm - Removes a file or directory
touch - creates a file
cat - (limited functionality) displays the contents of a file
cp2l - Copies a file from the test file system to the linux file system
cp2fs - Copies a file from the Linux file system to the test file system
cd - Changes directory
pwd - Prints the working directory
history - Prints out the history
help - Prints out help
```


This is deliberately vague, as it is dependent on your filesystem design.  And this all you may get initially for a real-world assignment, so if you have questions, please ask.

We will discuss some of this in class.

For our purposes use 10,000,000 or less (minimum 500,000) bytes for the volume size and 512 bytes per sector.  These are the values to pass into startPartitionSystem.

What needs to be submitted (via GitHub):

* 	All source files (.c and .h)
* 	Modified Driver program (must be a program that just utilizes the header file for your file system).
* 	The Driver program must be named:  `fsshell.c` (as provided)
* 	A make file (named “Makefile”) to build your entire program
 
* A PDF writeup on project that should include (this is also submitted in **Canvas**):
	* The github link for your group submission.
	* The plan/approach for each phase and changes made
	* A description of your file system
	* A list of exactly which shell commands work and don't work or limitations
	* Issues you had
	* Details of how each of your functions work
 	* A detailed table of who worked on what, down to the individual function level
	* Screen shots showing each of the commands listed above
* 	Your volume file (limit 10MB)
*  There will also be an INDIVIDUAL report (form) to complete.
