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

void memcpy(void *dst, void *src, unsigned size)
{
	unsigned i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
}

void memset(void *dst, unsigned char c, unsigned size)
{
	unsigned i;

	for (i=0; i<size; i++)
		((unsigned char *)dst)[i] = c;
}

int strcmp(char *str1, char *str2)
{
	while (*str1 == *str2) {
		if (*str1 == '\0')
			return 1;
		str1++;
		str2++;
	}
	return 0;
}

char *strncpy(char *dest, const char *src, unsigned size)
{
	unsigned i;
	for (i=0; i<size && src[i] != '\0'; ++i)
		dest[i] = src[i];
	while (i < size)
		dest[i++] = '\0';
	return dest;
}

