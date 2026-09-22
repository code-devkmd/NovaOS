#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

void mouse_irq_handler(void);
int mouse_init(void);
int mouse_available(void);
int mouse_x(void);
int mouse_y(void);
uint8_t mouse_buttons(void);
void mouse_cursor_draw(void);
void mouse_cursor_hide(void);

#endif
