/*
 * src/memory_utils.c
 *
 * memory utilities
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
