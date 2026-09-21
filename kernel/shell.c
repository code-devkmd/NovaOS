#include "shell.h"
#include "terminal.h"
#include "keyboard.h"
#include "string.h"
#include "commands.h"
#include "fs.h"

#define COMMAND_SIZE 128
#define HISTORY_SIZE 8

static char command[COMMAND_SIZE];
static char history[HISTORY_SIZE][COMMAND_SIZE];
static int history_count;
static int history_index;
static int command_length;
static int cursor_index;
static size_t line_row;
static size_t line_column;
static int displayed_length;

static const char *const command_names[] =
{
    "help", "clear", "echo", "ls", "cd", "pwd", "mkdir",
    "touch", "write", "cat", "rm", "cp", "mv"
};

static void copy_string(char *dst, const char *src, int max)
{
    int i = 0;
    if (max <= 0) return;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

static void prompt(void)
{
    terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
    terminal_write("nandeshore@nova:");
    terminal_write(fs_pwd());
    terminal_write("$ ");
    terminal_setcolor(VGA_COLOR_WHITE);
    terminal_get_cursor(&line_row, &line_column);
}

static int available_line_space(void)
{
    int available = (int)VGA_WIDTH - (int)line_column - 1;
    if (available < 0) available = 0;
    if (available > COMMAND_SIZE - 1) available = COMMAND_SIZE - 1;
    return available;
}

static void redraw_command(void)
{
    int i;
    int clear_length = displayed_length > command_length ? displayed_length : command_length;

    terminal_set_cursor(line_row, line_column);

    for (i = 0; i < command_length; i++)
        terminal_putchar(command[i]);

    for (i = command_length; i < clear_length; i++)
        terminal_putchar(' ');

    displayed_length = command_length;
    terminal_set_cursor(line_row, line_column);
    for (i = 0; i < cursor_index; i++)
        terminal_cursor_right();
}

static void reset_command(void)
{
    command[0] = 0;
    command_length = 0;
    cursor_index = 0;
    displayed_length = 0;
    history_index = history_count;
}

static void save_history(void)
{
    int i;
    if (command_length == 0) return;

    if (history_count > 0 && strcmp(history[history_count - 1], command) == 0)
    {
        history_index = history_count;
        return;
    }

    if (history_count < HISTORY_SIZE)
    {
        copy_string(history[history_count], command, COMMAND_SIZE);
        history_count++;
    }
    else
    {
        for (i = 1; i < HISTORY_SIZE; i++)
            copy_string(history[i - 1], history[i], COMMAND_SIZE);
        copy_string(history[HISTORY_SIZE - 1], command, COMMAND_SIZE);
    }
    history_index = history_count;
}

static void load_history(int index)
{
    int i;
    if (history_count == 0) return;
    if (index < 0) index = 0;
    if (index >= history_count) index = history_count;

    if (index == history_count)
        command[0] = 0;
    else
        copy_string(command, history[index], COMMAND_SIZE);

    command_length = (int)strlen(command);
    if (command_length > available_line_space())
        command_length = available_line_space();
    command[command_length] = 0;
    cursor_index = command_length;
    history_index = index;
    redraw_command();
}

static void complete_command(void)
{
    int i;
    int matches = 0;
    int match = -1;
    int prefix_len = command_length;

    if (cursor_index != command_length) return;

    for (i = 0; i < (int)(sizeof(command_names) / sizeof(command_names[0])); i++)
    {
        if (strncmp(command_names[i], command, (unsigned int)prefix_len) == 0)
        {
            matches++;
            match = i;
        }
    }

    if (matches == 1 && match >= 0)
    {
        const char *name = command_names[match];
        int needed = (int)strlen(name) - prefix_len;
        if (needed > 0 && command_length + needed <= available_line_space())
        {
            int j;
            for (j = 0; j < needed; j++)
                command[command_length + j] = name[prefix_len + j];
            command_length += needed;
            command[cursor_index = command_length] = 0;
            redraw_command();
        }
        return;
    }

    if (matches > 1)
    {
        terminal_putchar('\n');
        for (i = 0; i < (int)(sizeof(command_names) / sizeof(command_names[0])); i++)
        {
            if (strncmp(command_names[i], command, (unsigned int)prefix_len) == 0)
            {
                terminal_write(command_names[i]);
                terminal_write("  ");
            }
        }
        terminal_putchar('\n');
        prompt();
        redraw_command();
    }
}

static int parse(char *line, char **argv)
{
    int argc = 0;
    char *p = line;
    while (*p && argc < 10)
    {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (*p == '"')
        {
            p++;
            argv[argc++] = p;
            while (*p && *p != '"') p++;
            if (*p) *p++ = 0;
        }
        else
        {
            argv[argc++] = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = 0;
        }
    }
    return argc;
}

static void execute(char *line)
{
    char *argv[10];
    int argc = parse(line, argv);
    if (argc == 0) return;

    if (strcmp(argv[0], "help") == 0) cmd_help();
    else if (strcmp(argv[0], "clear") == 0) cmd_clear();
    else if (strcmp(argv[0], "echo") == 0)
    {
        int i;
        for (i = 1; i < argc; i++)
        {
            if (i > 1) terminal_putchar(' ');
            terminal_write(argv[i]);
        }
        terminal_putchar('\n');
    }
    else if (strcmp(argv[0], "ls") == 0)
        cmd_fs_ls(argc > 1 ? argv[1] : ".");
    else if (strcmp(argv[0], "cd") == 0)
        cmd_fs_cd(argc > 1 ? argv[1] : "/");
    else if (strcmp(argv[0], "pwd") == 0)
        cmd_fs_pwd();
    else if (strcmp(argv[0], "mkdir") == 0)
    {
        if (argc < 2) terminal_write("usage: mkdir PATH\n");
        else cmd_fs_mkdir(argv[1]);
    }
    else if (strcmp(argv[0], "touch") == 0)
    {
        if (argc < 2) terminal_write("usage: touch FILE\n");
        else cmd_fs_touch(argv[1]);
    }
    else if (strcmp(argv[0], "write") == 0)
    {
        int i, pos = 0;
        char text[FS_MAX_FILE_SIZE + 1];
        if (argc < 3)
        {
            terminal_write("usage: write FILE TEXT\n");
            return;
        }
        for (i = 2; i < argc && pos < FS_MAX_FILE_SIZE; i++)
        {
            int j = 0;
            if (i > 2 && pos < FS_MAX_FILE_SIZE) text[pos++] = ' ';
            while (argv[i][j] && pos < FS_MAX_FILE_SIZE) text[pos++] = argv[i][j++];
        }
        text[pos] = 0;
        cmd_fs_write(argv[1], text);
    }
    else if (strcmp(argv[0], "cat") == 0)
    {
        if (argc < 2) terminal_write("usage: cat FILE\n");
        else cmd_fs_cat(argv[1]);
    }
    else if (strcmp(argv[0], "rm") == 0)
    {
        if (argc < 2) terminal_write("usage: rm PATH\n");
        else cmd_fs_rm(argv[1]);
    }
    else if (strcmp(argv[0], "cp") == 0)
    {
        if (argc < 3) terminal_write("usage: cp SRC DST\n");
        else cmd_fs_cp(argv[1], argv[2]);
    }
    else if (strcmp(argv[0], "mv") == 0)
    {
        if (argc < 3) terminal_write("usage: mv SRC DST\n");
        else cmd_fs_mv(argv[1], argv[2]);
    }
    else
        terminal_write("command not found\n");
}

void shell_start(void)
{
    history_count = 0;
    history_index = 0;
    reset_command();
    terminal_write("\n");
    prompt();
}

void shell_run(void)
{
    while (1)
    {
        uint16_t key = keyboard_getkey();

        if (key == KEY_NONE) continue;

        if (key == KEY_LEFT)
        {
            if (cursor_index > 0) { cursor_index--; terminal_cursor_left(); }
            continue;
        }

        if (key == KEY_RIGHT)
        {
            if (cursor_index < command_length) { cursor_index++; terminal_cursor_right(); }
            continue;
        }

        if (key == KEY_HOME)
        {
            while (cursor_index > 0) { cursor_index--; terminal_cursor_left(); }
            continue;
        }

        if (key == KEY_END)
        {
            while (cursor_index < command_length) { cursor_index++; terminal_cursor_right(); }
            continue;
        }

        if (key == KEY_UP)
        {
            if (history_count > 0 && history_index > 0)
                load_history(history_index - 1);
            continue;
        }

        if (key == KEY_DOWN)
        {
            if (history_index < history_count)
                load_history(history_index + 1);
            continue;
        }

        if (key == KEY_DELETE)
        {
            if (cursor_index < command_length)
            {
                int i;
                for (i = cursor_index; i < command_length; i++) command[i] = command[i + 1];
                command_length--;
                redraw_command();
            }
            continue;
        }

        if (key == KEY_CTRL_C)
        {
            command[0] = 0;
            command_length = 0;
            cursor_index = 0;
            redraw_command();
            terminal_write("^C\n");
            prompt();
            continue;
        }

        if (key == '\t')
        {
            complete_command();
            continue;
        }

        if (key == '\b')
        {
            if (cursor_index > 0)
            {
                int i;
                for (i = cursor_index - 1; i < command_length; i++) command[i] = command[i + 1];
                command_length--;
                cursor_index--;
                redraw_command();
            }
            continue;
        }

        if (key == '\n')
        {
            command[command_length] = 0;
            terminal_putchar('\n');
            save_history();
            execute(command);
            reset_command();
            prompt();
            continue;
        }

        if (key < 0x100 && key >= 32 && key <= 126)
        {
            int limit = available_line_space();
            if (command_length < limit && command_length < COMMAND_SIZE - 1)
            {
                int i;
                for (i = command_length; i > cursor_index; i--) command[i] = command[i - 1];
                command[cursor_index] = (char)key;
                command_length++;
                cursor_index++;
                command[command_length] = 0;
                redraw_command();
            }
        }
    }
}
