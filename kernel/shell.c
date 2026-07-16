#include "shell.h"
#include "terminal.h"
#include "keyboard.h"
#include "string.h"
#include "commands.h"

static char command[128];

void shell_start(void) {
    terminal_write("NovaOS> ");
}

void shell_run(void) {
    
    int index = 0;

    while (1) {

        char c = keyboard_getchar();
    
        if (!c)
            continue;

        
        if (c == '\b') {

            if (index > 0) {

                index--;
                command[index] = 0;
                terminal_putchar('\b');

            }

            continue;
        }

        if (c == '\n') {

            command[index] = 0;

            terminal_putchar('\n');

            if (strcmp(command, "help") == 0)
            {
                cmd_help();
            }
            else if (strcmp(command, "clear") == 0)
            {
                cmd_clear();
            }
            else if (strcmp(command, "about") == 0)
            {
                cmd_about();
            }
            else if (strcmp(command, "echo") == 0)
            {
                cmd_echo("");
            }
            else
            {
                terminal_write("Unknown command\n");
            }

            index = 0;

            shell_start();

            continue;
        }

        if (index < 127) {
            command[index++] = c;
            terminal_putchar(c);
        }
    }
}