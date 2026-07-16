#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>

void terminal_clear(void);
void terminal_putchar(char c);
void terminal_write(const char *str);
void terminal_setcolor(unsigned char color);

#endif