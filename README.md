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

```
int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

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


