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

void malloc_dump(void *start, size_t size)
{
	int total_mallocs=0, free_mallocs=0, used_mallocs=0;
	size_t free_mem=0, used_mem=0;
	struct malloc_t *first_seg = (struct malloc_t *)HEAP;
	struct list_t *iter, *heap_list=first_seg->heap_list.prev;
	list_foreach(heap_list, iter) {
		struct malloc_t *cur_malloc = container_of(iter, struct malloc_t, heap_list);
		++total_mallocs;
		if (cur_malloc->free == 0) {
			++used_mallocs;
			used_mem += cur_malloc->size;
		} else {
			++free_mallocs;
			free_mem += cur_malloc->size;
		}
	}
	printf("Segments:\t%d free (%lu bytes)\t%d used (%lu bytes)\n", free_mallocs, free_mem, used_mallocs, used_mem);
	printf("Consistency:\tSegmentation [%s]\tMemory [%s]\n", (free_mallocs+used_mallocs == total_mallocs) ? "OK" : "BAD", (used_mem+free_mem+total_mallocs*sizeof(struct malloc_t) == size) ? "OK" : "BAD");
}

int main(void) {
	int i;
	void *ptrs[8];

	printf("Initalizing heap\n================\n");
	malloc_init(HEAP, HEAP_SIZE);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing random memory\n=====================\n");
	free(HEAP);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Making 8 allocations of 64 bytes\n================================\n");
	for (i=0; i<8; i++)
		ptrs[i] = malloc(64);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=1; i<8; i+=2)
		free(ptrs[i]);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=0; i<8; i+=2)
		free(ptrs[i]);
	malloc_dump(HEAP, HEAP_SIZE);

	return 0;
}
