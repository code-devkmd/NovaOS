#include "commands.h"
#include "terminal.h"

void cmd_help(void) {
    terminal_write("Commands:\n help\n clear\n echo\n about\n");
}

void cmd_clear(void) {
    terminal_clear();
}

void cmd_about(void) {
    terminal_write("NovaOS v0.3\n");
    terminal_write("A hobby operating system written in C\n\t Built by Nandeshore.\n");
}

void cmd_echo(char *args) {
    terminal_write(args);
    terminal_putchar('\n');
}