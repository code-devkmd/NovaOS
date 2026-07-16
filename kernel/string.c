#include "string.h"

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len])
        len++;

    return len;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return *a - *b;

        a++;
        b++;
    }

    return *a - *b;
}