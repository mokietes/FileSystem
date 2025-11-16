```
int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir * fs_opendir(const char *pathname);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

