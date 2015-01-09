/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/string.h
 *
 * String handling
 *
 * Author:  Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:  March 7 2014
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef STRING_H
#define STRING_H

#include <types.h>

void *memcpy(void *dst, const void *src, unsigned long size);
void *memset(void *dst, int c, unsigned long size);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strncpy(char *dst, const char *src, unsigned long n);
size_t strlen(const char *str);
int toupper(int c);

#endif /* STRING_H */
