/*
 * src/memory_utils.c
 *
 * memory utilities
 */

void memcpy(void *dst, void *src, unsigned size)
{
	int i;

	for (i=0; i<size; i++)
		((char *)dst)[i] = ((char *)src)[i];
}

void memset(void *dst, char c, unsigned size)
{
	int i;

	for (i=0; i<size; i++)
		((char *)dst)[i] = c;
}
