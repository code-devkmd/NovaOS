#ifndef COMMANDS_H
#define COMMANDS_H

void cmd_help(void);
void cmd_clear(void);
void cmd_echo(char *args);

void cmd_fs_ls(const char *path);
void cmd_fs_cd(const char *path);
void cmd_fs_pwd(void);
void cmd_fs_mkdir(const char *path);
void cmd_fs_touch(const char *path);
void cmd_fs_write(const char *path, const char *text);
void cmd_fs_cat(const char *path);
void cmd_fs_rm(const char *path);
void cmd_fs_cp(const char *src, const char *dst);
void cmd_fs_mv(const char *src, const char *dst);

#endif
