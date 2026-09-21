#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

#define FS_MAX_NAME 31
#define FS_MAX_PATH 255
#define FS_MAX_FILE_SIZE 8192

int fs_init(void);
int fs_format(void);
int fs_sync(void);
int fs_is_mounted(void);

int fs_ls(const char *path);
int fs_cd(const char *path);
const char *fs_pwd(void);
int fs_mkdir(const char *path);
int fs_touch(const char *path);
int fs_write(const char *path, const char *data);
int fs_cat(const char *path);
int fs_rm(const char *path);
int fs_cp(const char *src, const char *dst);
int fs_mv(const char *src, const char *dst);
int fs_stat(const char *path);
int fs_find(const char *path);
int fs_grep(const char *needle, const char *path);
int fs_mount(void);

#endif
