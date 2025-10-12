#define _DEFAULT_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parsePath.h"
#include "mfs.h"

#define DIRMAX_LEN 4096

/*
 * Opens a directory given a pathname.
 * Loads the directory block from disk and prepares for iteration.
 * Returns a directory handle (fdDir pointer), or NULL on error.
 */
fdDir *fs_opendir(const char *pathname)
{
    if (!pathname) return NULL;

    ppInfo info;
    char *pathCopy = strdup(pathname);
    if (parsePath(pathCopy, &info) != 0) {
        return NULL;
    }

    dirEntry *dirBlock = NULL;

    if (info.index == -2) {
        dirBlock = rootDir;
    } else if (info.index != -1 && isDirEntryDir(&info.parent[info.index])) {
        dirBlock = loadDir(&info.parent[info.index]);
    }

    if (!dirBlock) return NULL;

    fdDir *newfdDir = malloc(sizeof(fdDir));
    newfdDir->d_reclen = dirBlock[0].size;
    newfdDir->dirEntryPosition = 0;
    newfdDir->directory = dirBlock;

    return newfdDir;
}


/*
 * Reads one valid entry from an opened directory.
 * Returns a pointer to static fs_diriteminfo, or NULL if done.
 */
struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    if (dirp == NULL) {
        return NULL;
    }

    struct fs_diriteminfo *di = malloc(sizeof(struct fs_diriteminfo));

    int entryCount = dirp->directory[0].size / sizeof(dirEntry);

    while (dirp->dirEntryPosition < entryCount) {
        // Skip unused entries
        if (dirp->directory[dirp->dirEntryPosition].name[0] == '\0') {
            dirp->dirEntryPosition++;
            continue;
        }

        di->d_reclen = (unsigned short) sizeof(struct fs_diriteminfo);
        di->fileType = (unsigned char) dirp->directory[dirp->dirEntryPosition].isDir ?
            FT_DIRECTORY : FT_REGFILE;
        strcpy(di->d_name, dirp->directory[dirp->dirEntryPosition].name);
        di->d_name[255] = '\0';

        dirp->dirEntryPosition++;

        return di;
    }

    return NULL;
}

/*
 * Closes an open directory stream.
 * Frees the internal structure and loaded directory if needed.
 */
int fs_closedir(fdDir *dirp) {

    if (dirp == NULL) {
        printf("Cannot close because directory is null\n");
        return 0;
    }

    free(dirp);
    return 0;
}

/*
 * Returns a pointer to the current working directory path.
 * If `buf` is valid and `size` allows, the path is copied to it.
 * Returns NULL on error, or the buffer pointer on success.
 */


char *fs_getcwd(char *buf, size_t size)
{
    if (!buf || size < 2) return NULL;

    // Handle if current working directory is root
    if (cwDir == rootDir || cwDir[0].blockLoc == rootDir[0].blockLoc) {
        strncpy(buf, "/" ,size);
        buf[size - 1] = '\0';  // Ensure null-termination

        return buf;
    }

    dirEntry *current = cwDir;
    char tempPath[DIRMAX_LEN] = "";
    int tempPos = 0;
    int loopCounter = 0;

    while (current != NULL) {

        if (current == rootDir || current[0].blockLoc == rootDir[0].blockLoc) {
            if (tempPos == 0) {
                tempPath[0] = '/';
                tempPath[1] = '\0';
                tempPos = 1;
            } 
            break;
        }

        if (current[1].isDir != 1) {
            return NULL;
        }

        dirEntry *parent = loadDir(&current[1]);
        if (parent == NULL) {

            return NULL;
        }

        int found = 0;

        int entries = parent[0].size / sizeof(dirEntry);
        
        for (int i = 2; i < entries; i++) {
            if (parent[i].name[0] == '\0') continue;

            if (parent[i].blockLoc == current[0].blockLoc) {
                size_t len = strlen(parent[i].name);
                if (tempPos + len + 2 > sizeof(tempPath)) {
                    safeFree(parent);
                    return NULL;
                }

                // Shift existing path to the right
                if (tempPos > 0) {
                    memmove(tempPath + len + 1, tempPath, tempPos + 1);
                    tempPath[len] = '/';
                // otherwise null terminate the existing path
                } else {
                    tempPath[len] = '\0';
                }

                memcpy(tempPath, parent[i].name, len);
                tempPos += len + (tempPos > 0 ? 1 : 0);
                found = 1;
                break;
            }
        }

        safeFree(current);
        current = parent;

        if (found == 0) {
            return NULL;
        }

        loopCounter++;
    }

    if (tempPos + 1 >= size) {
        return NULL;
    }

    buf[0] = '/';
    strncpy(buf + 1, tempPath, size - 2);
    buf[size - 1] = '\0';

    return buf;
}

/*
* returns 0 on success
* returns -1 on error
*/

int fs_setcwd(char *pathname) {
    if (pathname == NULL) {
        return -1;
    }

    ppInfo ppi;
    int ppRet = parsePath(pathname, &ppi);

    // Check if path is valid
    if (ppRet != 0) {
        return -1;
    }

    // Handle root directory case
    if (ppi.index == -2) {
        if (cwDir != rootDir) {
            safeFree(cwDir);
            cwDir = rootDir;
        }
        safeFree(ppi.parent);
        return 0;
    }

    // Check if last element exists
    if (ppi.index == -1) {
        safeFree(ppi.parent);
        return -1;
    }

    // Verify it's a directory
    if (fs_isDir(pathname) != 1) {
        safeFree(ppi.parent);
        return -1;
    }

    // Load the new directory
    dirEntry *newDir = loadDir(&ppi.parent[ppi.index]);
    if (newDir == NULL) {
        safeFree(ppi.parent);
        return -1;
    }

    // Update current working directory
    safeFree(cwDir);
    cwDir = newDir;

    safeFree(ppi.parent);
    return 0;
}


/*
 * Returns 1 if the given path resolves to a regular file.
 * Returns 0 if path is invalid, not found, or is a directory.
 */
