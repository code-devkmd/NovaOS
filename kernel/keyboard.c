#include "keyboard.h"
#include "terminal.h"

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static const char keymap[128] = {
    0,
    27,     // Esc
    '1','2','3','4','5','6','7','8','9','0',
    '-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p',
    '[',']',
    '\n',
    0,      // Ctrl
    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',
    0,      // Left Shift
    '\\',
    'z','x','c','v','b','n','m',
    ',', '.', '/',
    0,      // Right Shift
    '*',
    0,      // Alt
    ' ',
};

char keyboard_getchar(void)
{
    unsigned char scancode = inb(0x60);

    // Ignore key releases
    if (scancode & 0x80)
        return 0;

    if (scancode < sizeof(keymap))
        return keymap[scancode];

    return 0;
}