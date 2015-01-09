/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/string.c
 *
 * C string functions
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	March 7 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <types.h>

void *memcpy(void *dst, const void *src, unsigned long size)
{
	size_t i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = ((unsigned char *)src)[i];

	return dst;
}

void *memset(void *dst, int c, unsigned long size)
{
	size_t i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = c;

	return dst;
}

int strcmp(const char *str1, const char *str2)
{
	const char *p1 = str1;
	const char *p2 = str2;
	while (*p1 == *p2) {
		if (*p1 == '\0')
			return 0;
		p1++;
		p2++;
	}
	return *p1 > *p2 ? 1 : -1;
}

int strncmp(const char *str1, const char *str2, size_t n)
{
	int i = 0;
	while (str1[i] == str2[i]) {
		if (str1[i] == '\0' || i == n-1)
			return 0;
		++i;
	}
	return str1[i] > str2[i] ? 1 : -1;
}

char *strncpy(char *dst, const char *src, unsigned long n)
{
	size_t i;
	for (i=0; i<n && src[i] != '\0'; i++)
		dst[i] = src[i];
	while (i < n)
		dst[i++] = '\0';
	return dst;
}

size_t strlen(const char *str)
{
	int len;
	for (len=0; str[len] != '\0'; len++)
		;
	return len;
}

int toupper(int c)
{
	return (c > 96 && c < 123) ? c - ('a' - 'A') : c;
}
