#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *str);

int strcmp(const char *a, const char *b);

int strncmp(const char *a, const char *b, unsigned int n);
int strstr(const char *haystack, const char *needle);

#endif