/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/malloc.c
 *
 * Simple malloc implementation
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	September 25 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <stdio.h>

#include "malloc.h"

static struct list_t malloc_list; /* -> heap_list */
static struct list_t free_list;
static struct list_t used_list;

/*
 * Initialize the heap
 */
void malloc_init(void *start, size_t size)
{
	struct malloc_t *seg = (struct malloc_t *)start;
	list_init(&malloc_list);
	list_init(&free_list);
	list_init(&used_list);
	list_insert_after(&malloc_list, &seg->list);
	list_insert_after(&free_list, &seg->f_list);
	seg->size = size - sizeof(struct malloc_t);
	seg->free = 1;
}

/*
 * First fit allocation
 */
static inline struct malloc_t *get_first_free_seg(size_t size)
{
	struct malloc_t *free_seg;
	list_find_item(free_seg, &free_list, f_list, free_seg->size >= size);
	return free_seg;
}

/*
 * Allocate memory
 */
void *malloc(size_t size)
{
	struct malloc_t *new_seg, *free_seg = get_free_seg(size);
	if (free_seg == NULL)
		return NULL;	/* unable to allocate size bytes */
	if (free_seg->size-size >= sizeof(struct malloc_t)) {
		new_seg = (struct malloc_t *)((size_t)(free_seg+1) + size);
		new_seg->free = 1;
		new_seg->size = free_seg->size - size - sizeof(struct malloc_t);
		list_insert_after(&free_seg->list, &new_seg->list);
		list_insert_after(&free_list, &new_seg->f_list);
		free_seg->size = size;
	}
	free_seg->free = 0;
	list_remove(&free_seg->f_list);
	list_insert_after(&used_list, &free_seg->f_list);
	return (void *)(free_seg + 1);
}

/*
 * Free allocated memory
 */
void free(void *ptr)
{
	struct malloc_t *prev_seg, *next_seg, *ptr_seg;
	list_find_item(ptr_seg, &used_list, f_list, (void *)(ptr_seg+1) == ptr);
	if (ptr_seg == NULL)
		return; /* FIXME: fails silently */
	list_remove(&ptr_seg->f_list);
	prev_seg = list_prev_item(ptr_seg, list);
	next_seg = list_next_item(ptr_seg, list);
	if (&next_seg->list != &malloc_list && next_seg->free) {
		ptr_seg->size += next_seg->size + sizeof(struct malloc_t);
		list_remove(&next_seg->list);
		list_remove(&next_seg->f_list);
	}
	if (&prev_seg->list != &malloc_list && prev_seg->free) {
		prev_seg->size += ptr_seg->size + sizeof(struct malloc_t);
		list_remove(&ptr_seg->list);
	} else {
		ptr_seg->free = 1;
		list_insert_after(&free_list, &ptr_seg->f_list);
	}
}

void malloc_dump(void *start, size_t size)
{
	int total_mallocs=0, free_mallocs=0, used_mallocs=0;
	size_t free_mem=0, used_mem=0;
	struct list_t *iter;
	printf("Heap size: %lu\n", size);
	list_foreach(&malloc_list, iter) {
		struct malloc_t *cur_malloc = container_of(iter, struct malloc_t, list);
		++total_mallocs;
		if (cur_malloc->free == 0) {
			++used_mallocs;
			used_mem += cur_malloc->size;
		} else {
			++free_mallocs;
			free_mem += cur_malloc->size;
		}
	}
	printf("Free allocations: %d (%lu bytes)\n", free_mallocs, free_mem);
	printf("Used allocations: %d (%lu bytes)\n", used_mallocs, used_mem);
	printf("Allocation consistency: [%s]\n", (free_mallocs+used_mallocs == total_mallocs) ? "OK" : "BAD");
	printf("Memory consistency: [%s]\n", (used_mem+free_mem+total_mallocs*sizeof(struct malloc_t) == size) ? "OK" : "BAD");
}
