#include "commands.h"
#include "terminal.h"
#include "fs.h"

void cmd_help(void)
{
    terminal_write("Commands:\n");
    terminal_write("  ls cd pwd mkdir touch\n");
    terminal_write("  write cat rm cp mv\n");
    terminal_write("  clear echo help\n");
}

void cmd_clear(void)
{
    terminal_clear();
}

void cmd_echo(char *args)
{
    terminal_write(args);
    terminal_putchar('\n');
}

static void fs_result(int r)
{
    if (r == 0) return;
    if (r == -2)
        terminal_write("error: directory not empty\n");
    else
        terminal_write("error: operation failed\n");
}

void cmd_fs_ls(const char *path) { fs_result(fs_ls(path)); }
void cmd_fs_cd(const char *path) { fs_result(fs_cd(path)); }

void cmd_fs_pwd(void)
{
    terminal_write(fs_pwd());
    terminal_putchar('\n');
}

void cmd_fs_mkdir(const char *path) { fs_result(fs_mkdir(path)); }
void cmd_fs_touch(const char *path) { fs_result(fs_touch(path)); }
void cmd_fs_write(const char *path, const char *text) { fs_result(fs_write(path, text)); }
void cmd_fs_cat(const char *path) { fs_result(fs_cat(path)); }
void cmd_fs_rm(const char *path) { fs_result(fs_rm(path)); }
void cmd_fs_cp(const char *src, const char *dst) { fs_result(fs_cp(src, dst)); }
void cmd_fs_mv(const char *src, const char *dst) { fs_result(fs_mv(src, dst)); }
