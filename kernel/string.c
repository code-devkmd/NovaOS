#include "string.h"

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len])
    {
        len++;
    }

    return len;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
        {
            return *a - *b;
        }

        a++;
        b++;
    }

    return *a - *b;
}

int strncmp(const char *a, const char *b, unsigned int n)
{
    while (n > 0 && *a && *b)
    {
        if (*a != *b)
        {
            return *a - *b;
        }

        a++;
        b++;
        n--;
    }

    if (n == 0)
    {
        return 0;
    }

    return *a - *b;
}
int strstr(const char *haystack, const char *needle)
{
    size_t n = strlen(needle);
    if (n == 0) return 1;
    while (*haystack)
    {
        size_t i = 0;
        while (i < n && haystack[i] == needle[i]) i++;
        if (i == n) return 1;
        haystack++;
    }
    return 0;
}
