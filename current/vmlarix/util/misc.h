
#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>

void panic();

size_t strlen(const char *s);

char *strdup(const char *s);

char *strchr(char *s,int c);

char *strncpy(char *dest, const char *src, size_t n);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

void int_to_hex(int val, char *buf);

#endif
