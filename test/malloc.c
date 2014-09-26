/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/malloc.c
 *
 * Tests for malloc
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 26 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <stdio.h>
#include <malloc.h>

#define HEAP_SIZE 1024

char HEAP[HEAP_SIZE];

struct malloc_info_t {
	int total_seg;
	int free_seg;
	int used_seg;
	size_t total_mem;
	size_t free_mem;
	size_t used_mem;
};

struct malloc_info_t malloc_info()
{
	struct malloc_info_t info = { 0, 0, 0, 0, 0, 0 }, *pinfo=&info;
	struct list_t *iter, *heap_list=((struct malloc_t *)HEAP)->heap_list.prev;
	list_foreach(heap_list, iter) {
		struct malloc_t *cur_malloc = container_of(iter, struct malloc_t, heap_list);
		++pinfo->total_seg;
		if (cur_malloc->free == 0) {
			++pinfo->used_seg;
			pinfo->used_mem += cur_malloc->size;
		} else {
			++pinfo->free_seg;
			pinfo->free_mem += cur_malloc->size;
		}
	}
	return info;
}

int main(void) {
	int i;
	void *ptrs[8];

	printf("Initalizing heap\n================\n");
	malloc_init(HEAP, HEAP_SIZE);

	printf("Freeing random memory\n=====================\n");
	free(HEAP);

	printf("Making 8 allocations of 64 bytes\n================================\n");
	for (i=0; i<8; i++)
		ptrs[i] = malloc(64);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=1; i<8; i+=2)
		free(ptrs[i]);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=0; i<8; i+=2)
		free(ptrs[i]);

	return 0;
}
