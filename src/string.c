/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/memory_utils.c
 *
 * memory utilities
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

void memcpy(void *dst, void *src, size_t size)
{
	size_t i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
}

void memset(void *dst, unsigned char c, size_t size)
{
	size_t i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = c;
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

char *strncpy(char *dest, const char *src, size_t size)
{
	size_t i;
	for (i=0; i<size && src[i] != '\0'; i++)
		dest[i] = src[i];
	while (i < size)
		dest[i++] = '\0';
	return dest;
}

