#include "keyboard.h"

#define KEYBOARD_DATA   0x60
#define KEYBOARD_BUFFER_SIZE 128

static volatile uint16_t buffer[KEYBOARD_BUFFER_SIZE];
static volatile unsigned int head;
static volatile unsigned int tail;
static int shift_pressed;
static int ctrl_pressed;
static int extended_prefix;

static const char keymap[128] =
{
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

static const char keymap_shift[128] =
{
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '
};

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void buffer_push(uint16_t key)
{
    unsigned int next = (head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next == tail) return;
    buffer[head] = key;
    head = next;
}

void keyboard_init(void)
{
    head = 0;
    tail = 0;
    shift_pressed = 0;
    ctrl_pressed = 0;
    extended_prefix = 0;
}

void keyboard_irq_handler(void)
{
    unsigned char scancode = inb(KEYBOARD_DATA);
    int released = (scancode & 0x80) != 0;
    unsigned char code = scancode & 0x7F;

    if (scancode == 0xE0)
    {
        extended_prefix = 1;
        return;
    }

    if (released)
    {
        if (code == 0x2A || code == 0x36) shift_pressed = 0;
        if (code == 0x1D) ctrl_pressed = 0;
        extended_prefix = 0;
        return;
    }

    if (code == 0x2A || code == 0x36)
    {
        shift_pressed = 1;
        extended_prefix = 0;
        return;
    }

    if (code == 0x1D)
    {
        ctrl_pressed = 1;
        extended_prefix = 0;
        return;
    }

    if (extended_prefix)
    {
        extended_prefix = 0;
        switch (code)
        {
            case 0x48: buffer_push(KEY_UP); break;
            case 0x50: buffer_push(KEY_DOWN); break;
            case 0x4B: buffer_push(KEY_LEFT); break;
            case 0x4D: buffer_push(KEY_RIGHT); break;
            case 0x47: buffer_push(KEY_HOME); break;
            case 0x4F: buffer_push(KEY_END); break;
            case 0x53: buffer_push(KEY_DELETE); break;
            default: break;
        }
        return;
    }

    if (code >= 128) return;

    if (ctrl_pressed)
    {
        if (code == 0x2E) buffer_push(KEY_CTRL_C); /* C */
        return;
    }

    {
        char c = shift_pressed ? keymap_shift[code] : keymap[code];
        if (c) buffer_push((uint16_t)(unsigned char)c);
    }
}

uint16_t keyboard_getkey(void)
{
    uint16_t key;
    if (head == tail) return KEY_NONE;
    key = buffer[tail];
    tail = (tail + 1) % KEYBOARD_BUFFER_SIZE;
    return key;
}

char keyboard_getchar(void)
{
    uint16_t key = keyboard_getkey();
    if (key > 0xFF) return 0;
    return (char)key;
}
