#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEY_NONE   0U
#define KEY_UP     0x100U
#define KEY_DOWN   0x101U
#define KEY_LEFT   0x102U
#define KEY_RIGHT  0x103U
#define KEY_HOME   0x104U
#define KEY_END    0x105U
#define KEY_DELETE 0x106U
#define KEY_CTRL_C 0x107U

void keyboard_init(void);
void keyboard_irq_handler(void);
uint16_t keyboard_getkey(void);
char keyboard_getchar(void);

#endif
