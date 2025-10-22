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

/****   SET THESE TO 1 WHEN READY TO TEST THAT COMMAND ****/
#define CMDLS_ON	1
#define CMDCP_ON	1
#define CMDMV_ON	1
#define CMDMD_ON	1
#define CMDRM_ON	1
#define CMDCP2L_ON	1
#define CMDCP2FS_ON	1
#define CMDCD_ON	1
#define CMDPWD_ON	1
#define CMDTOUCH_ON	1
#define CMDCAT_ON	1


typedef struct dispatch_t
	{
	char * command;
	int (*func)(int, char**);
	char * description;
	} dispatch_t, * dispatch_p;


int cmd_ls (int argcnt, char *argvec[]);
int cmd_cp (int argcnt, char *argvec[]);
int cmd_mv (int argcnt, char *argvec[]);
int cmd_md (int argcnt, char *argvec[]);
int cmd_rm (int argcnt, char *argvec[]);
int cmd_touch (int argcnt, char *argvec[]);
int cmd_cat (int argcnt, char *argvec[]);
int cmd_cp2l (int argcnt, char *argvec[]);
int cmd_cp2fs (int argcnt, char *argvec[]);
int cmd_cd (int argcnt, char *argvec[]);
int cmd_pwd (int argcnt, char *argvec[]);
int cmd_history (int argcnt, char *argvec[]);
int cmd_help (int argcnt, char *argvec[]);

dispatch_t dispatchTable[] = {
	{"ls", cmd_ls, "Lists the file in a directory"},
	{"cp", cmd_cp, "Copies a file - source [dest]"},
	{"mv", cmd_mv, "Moves a file - source dest"},
	{"md", cmd_md, "Make a new directory"},
	{"rm", cmd_rm, "Removes a file or directory"},
        {"touch",cmd_touch, "Touches/Creates a file"},
        {"cat", cmd_cat, "Limited version of cat that displace the file to the console"},
	{"cp2l", cmd_cp2l, "Copies a file from the test file system to the linux file system"},
	{"cp2fs", cmd_cp2fs, "Copies a file from the Linux file system to the test file system"},
	{"cd", cmd_cd, "Changes directory"},
	{"pwd", cmd_pwd, "Prints the working directory"},
	{"history", cmd_history, "Prints out the history"},
	{"help", cmd_help, "Prints out help"}
};

static int dispatchcount = sizeof (dispatchTable) / sizeof (dispatch_t);

// Display files for use by ls command
int displayFiles (fdDir * dirp, int flall, int fllong)
	{
#if (CMDLS_ON == 1)				
	if (dirp == NULL)	//get out if error
		return (-1);
	
	struct fs_diriteminfo * di;
	struct fs_stat statbuf;
	
	di = fs_readdir (dirp);
	printf("\n");
	while (di != NULL) 
		{
		if ((di->d_name[0] != '.') || (flall)) //if not all and starts with '.' it is hidden
			{
			if (fllong)
				{
				fs_stat (di->d_name, &statbuf);
				printf ("%s    %9ld   %s\n", fs_isDir(di->d_name)?"D":"-", statbuf.st_size, di->d_name);
				}
			else
				{
				printf ("%s\n", di->d_name);
				}
			}
		di = fs_readdir (dirp);
		}
	fs_closedir (dirp);
#endif
	return 0;
	}
	

/****************************************************
*  ls commmand
****************************************************/
int cmd_ls (int argcnt, char *argvec[])
	{
#if (CMDLS_ON == 1)				
	int option_index;
	int c;
	int fllong;
	int flall;
	char cwd[DIRMAX_LEN];
		
