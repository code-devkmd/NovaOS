#include "keyboard.h"

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static int shift = 0;

static const char keymap[128] = {
    0,
    27,
    '1','2','3','4','5','6','7','8','9','0',
    '-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p',
    '[',']',
    '\n',
    0,
    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',
    0,
    '\\',
    'z','x','c','v','b','n','m',
    ',', '.', '/',
    0,
    '*',
    0,
    ' ',
};

static const char keymap_shift[128] = {
    0,
    27,
    '!','@','#','$','%','^','&','*','(',')',
    '_','+', '\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P',
    '{','}',
    '\n',
    0,
    'A','S','D','F','G','H','J','K','L',
    ':','"','~',
    0,
    '|',
    'Z','X','C','V','B','N','M',
    '<','>','?',
    0,
    '*',
    0,
    ' ',
};

char keyboard_getchar(void)
{
    static unsigned char last = 0;

    if (!(inb(0x64) & 1))
        return 0;

    unsigned char scancode = inb(0x60);

    if (scancode == last)
        return 0;

    last = scancode;

    // Left Shift press
    if (scancode == 0x2A)
    {
        shift = 1;
        return 0;
    }

    // Right Shift press
    if (scancode == 0x36)
    {
        shift = 1;
        return 0;
    }

    // Left Shift release
    if (scancode == 0xAA)
    {
        shift = 0;
        last = 0;
        return 0;
    }

    // Right Shift release
    if (scancode == 0xB6)
    {
        shift = 0;
        last = 0;
        return 0;
    }

    // Ignore other key releases
    if (scancode & 0x80)
    {
        last = 0;
        return 0;
    }

    if (scancode >= 128)
        return 0;

    if (shift)
        return keymap_shift[scancode];

    return keymap[scancode];
}