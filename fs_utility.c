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

